//
// Created by Hyunseung Ryu on 2025. 8. 4..
//

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "utils/log.hpp"
#include "utils/draw.hpp"
#include "utils/constants.hpp"
#include "control/dynamixel_actuator.hpp"
#include "control/linear_actuator.hpp"
#include "vision/bridge.hpp"
#include "vision/camera.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"



class ControlEnd {
private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const Bridge::Payload& payload) {
        const auto& [x, theta, swing_start, swing_end, use_right_hand] = payload;

        // 1) Move the linear actuator
        linearActuator.move_actu(x);

        // 2) Move to swing_start and wait
        bool swing_start_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_start},
            &swing_start_done
        );
        // swing_start_done == true here

        // 3) Only after swing_start is done, move to swing_end and wait
        bool swing_end_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_end},
            &swing_end_done
        );
        // swing_end_done == true here
    }

    void shutdown() {
        Bridge::Payload init{TABLE_WIDTH / 2, 90, 90, 90};
        actuators.bulk_move_by_degrees({init.theta, init.swing_start});
        linearActuator.move_actu(init.x);
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }

public:
    ControlEnd() : actuators({MID_SERIAL_PORT, BOT_SERIAL_PORT}) {
    }

    ~ControlEnd() {
        shutdown();
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& flag,
        const std::atomic<bool>& stop
    ) {
        if (!actuators.initialize()) {
            Log::error("[ControlEnd::run] Failed to initialize actuators.");
            return;
        }

        while (true) {
            std::unique_lock lock(queue_mutex);
            flag.wait(lock, [&] {
                return !queue.empty() || stop;
            });

            if (!queue.empty()) {
                Bridge::Payload payload = queue.front();
                queue.pop();
                lock.unlock();

                Log::debug(std::format(
                    "[ControlEnd::run] Received Payload with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
                    payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
                execute(payload);
            }
            else if (stop) {
                lock.unlock();

                Log::info("[ControlEnd::run] Stopping as stop signal received.");
                break;
            }
        }
    }
};

class VisionEnd {
private:
    Camera cam_top{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
    // Camera cam_top_2{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
    Tracker tracker_top{ORANGE_MIN, ORANGE_MAX};
    // Tracker tracker_top_2{ORANGE_MIN, ORANGE_MAX};
    Predictor predictor;

    cv::UMat latest_top_frame;
    // cv::UMat latest_top_frame_2;
    std::mutex frame_mutex;

public:
    VisionEnd() {
        cam_top.set_frame_callback([this](cv::UMat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_top << frame;
            if (const auto pos = tracker_top.get_camera_pos()) {
                predictor.set_point_top(pos.value().first);
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
                {10, 20}
            );
            frame.copyTo(latest_top_frame);
        });
    }

    ~VisionEnd() {
        cam_top.stop();
        cv::destroyAllWindows();
    }

    void read_frame(cv::UMat& frame_top) {
        std::lock_guard lock(frame_mutex);
        if (latest_top_frame.empty())
            return;

        latest_top_frame.copyTo(frame_top);
    }

    void process_data(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& flag,
        cv::Point3f& world_pos,
        cv::Vec3f& world_speed,
        cv::Point3f& predict_arrive_pos,
        cv::Point3f& real_arrive_pos,
        std::deque<cv::Point3f> orbit_3d,
        std::deque<cv::Point2f> orbit_2d_top
    ) {
        const double fallback_dt = 1.0 / std::min({cam_top.get_fps()});
        if (const auto find_world_pos = predictor.get_new_world_pos(static_cast<float>(fallback_dt))) {
            world_pos = find_world_pos.value();

            orbit_3d.push_back(world_pos);
            orbit_2d_top.push_back(predictor.pos_3d_to_2d(world_pos, CameraType::TOP));
            if (orbit_3d.size() > 100) {
                orbit_3d.pop_front();
                orbit_2d_top.pop_front();
            }
        }

        if (const auto find_world_speed = predictor.get_world_speed()) {
            world_speed = find_world_speed.value();
        }

        if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= PREDICT_MAX_Y) {
            if (const auto find_predict_pos = predictor.get_arrive_pos()) {
                predict_arrive_pos = find_predict_pos.value();
            }
        }
        else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
            0 <= world_pos.y && world_pos.y < PREDICT_MIN_Y &&
            0 <= world_pos.z
        ) {
            std::lock_guard lock(queue_mutex);
            if (queue.empty()) {
                Bridge::Payload payload = Bridge::convert(predict_arrive_pos, world_speed);
                queue.push(payload);
                flag.notify_one();

                Log::debug(std::format(
                    "[VisionEnd::run]: Payload set with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
                    payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
            }
        }

        if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
            std::abs(world_pos.y) < 5 &&
            0 <= world_pos.z) {
            real_arrive_pos = world_pos;
        }
    }

    void visualize_data(
        cv::UMat& frame_top,
        cv::UMat& frame_left,
        cv::UMat& frame_right,
        const cv::Point3f& world_pos,
        const cv::Vec3f& world_speed,
        const cv::Point3f& predict_arrive_pos,
        const cv::Point3f& real_arrive_pos,
        const std::deque<cv::Point3f>& orbit,
        const std::deque<cv::Point2f>& orbit_2d_top
    ) const {
        for (std::size_t i = 0; i < orbit.size(); ++i) {
            Draw::put_circle(frame_top, orbit_2d_top[i], 3, COLOR_MAGENTA);
        }

        const auto world_pos_2d_top = predictor.pos_3d_to_2d(world_pos, CameraType::TOP);

        Draw::put_circle(frame_top, world_pos_2d_top, 8, COLOR_GREEN);

        const auto world_speed_2d_top = predictor.pos_3d_to_2d(world_speed, CameraType::TOP);

        Draw::put_arrow(frame_top, world_pos_2d_top, world_pos_2d_top + world_speed_2d_top, COLOR_GREEN);

        Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::TOP), 8, COLOR_BLUE);

        Draw::put_circle(frame_top, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP), 8, COLOR_RED);
    }

    void legend(
        cv::UMat& frame,
        const cv::Point3f& world_pos,
        const cv::Vec3f& world_speed,
        const cv::Point3f& predict_arrive_pos,
        const cv::Point3f& real_arrive_pos
    ) const {
        Draw::put_text_border(frame, Draw::to_string("World Pos", world_pos, "cm"), {10, 50}, COLOR_GREEN);
        Draw::put_text_border(frame, Draw::to_string("World Speed", world_speed, "cm/s"), {10, 80}, COLOR_CYAN);
        Draw::put_text_border(frame, Draw::to_string("Predict Pos", predict_arrive_pos, "cm"), {10, 110}, COLOR_BLUE);
        Draw::put_text_border(frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), {10, 140}, COLOR_RED);
    }

    cv::UMat& combine_frame(const cv::UMat& frame_top_1, const cv::UMat& frame_top_2) {
        cv::UMat concatenated_frame;
        cv::hconcat(frame_top_1, frame_top_2, concatenated_frame);
        cv::resize(concatenated_frame, concatenated_frame, {}, 0.7, 0.7);

        return concatenated_frame;
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& flag,
        std::atomic<bool>& stop
    ) {
        if (!cam_top.is_opened()) {
            Log::error(std::format(
                "[VisionEnd::run] Failed to open cameras: top={}",
                cam_top.is_opened() ? "opened" : "failed"
            ));
            return;
        }

        cam_top.start();

        cv::Point3f world_pos{-1, -1, -1};
        cv::Vec3f world_speed{0, 0, 0};
        cv::Point3f predict_arrive_pos{-1, -1, -1};
        cv::Point3f real_arrive_pos{-1, -1, -1};
        std::deque<cv::Point3f> orbit_3d;
        std::deque<cv::Point2f> orbit_2d_top;

        while (true) {
            this->process_data(
                queue, queue_mutex, flag, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top
            );

            cv::UMat frame_top, frame_left, frame_right;
            this->read_frame(frame_top);
            this->visualize_data(
                frame_top, frame_left, frame_right, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top
            );

            cv::UMat frame_combined = frame_top; // this->combine_frame(frame_top, frame_top_2);
            this->legend(frame_combined, world_pos, world_speed, predict_arrive_pos, real_arrive_pos);

            cv::imshow("Pingpong Robot / Press 'Q' to quit, 'S' to save orbit.", frame_combined);

            const int key = cv::waitKey(1);
            if (key == 'q') {
                stop = true;
                flag.notify_all();
                Log::info("[VisionEnd::run] Stopping as 'q' key pressed.");

                break;
            }
            else if (key == 's') {
                cv::FileStorage fs(PATH_ORBIT, cv::FileStorage::WRITE);
                if (!fs.isOpened()) {
                    Log::error("[VisionEnd::run] Failed to open file for saving orbit.");
                }

                fs << "orbit_3d" << orbit_3d;
                fs << "orbit_2d_top" << orbit_2d_top;
                fs.release();
            }
        }
    }
};

int main() {
    VisionEnd vision_end;
    ControlEnd control_end;

    std::queue<Bridge::Payload> queue;
    std::mutex queue_mutex;
    std::condition_variable flag;
    std::atomic stop = false;

    auto producer = std::thread(&VisionEnd::run, &vision_end, std::ref(queue), std::ref(queue_mutex), std::ref(flag), std::ref(stop));
    auto consumer = std::thread(&ControlEnd::run, &control_end, std::ref(queue), std::ref(queue_mutex), std::ref(flag), std::ref(stop));

    producer.join();
    consumer.join();

    return 0;
}
