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
#include "control/robot_arm_controller.hpp"
#include "vision/bridge.hpp"
#include "vision/camera.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"

const cv::Point3f launcher_pos{TABLE_WIDTH / 2, TABLE_HEIGHT + 50.0f, 40.0f};
constexpr float launch_velocity = 335.0f;

class ControlEnd {
public:
    RobotArmController arm_controller_;
    LinearActuator linear_actuator_;

    float last_x = TABLE_WIDTH / 2;

    void execute(const Bridge::Payload& payload) {
        const auto& [x, steps] = payload;

        // 1) Move the linear actuator
        linear_actuator_.move_actu(x);
        const float dt_linear_ms = std::abs(x - last_x) / LINEAR_VELOCITY * 1000; // FIXME: LINEAR_VELOCITY 다시 재기
        last_x = x;

        Log::debug(std::format("Linear Actuator dt: {}s", dt_linear_ms / 1000));
        //std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt_linear_ms)));

        // 2) Move the arm
        arm_controller_.execute(steps);
    }

    void shutdown() {
        linear_actuator_.move_actu(TABLE_WIDTH / 2);
        arm_controller_.execute_each({90, 120, -80, 50});
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }

    ControlEnd() = default;

    ~ControlEnd() {
        shutdown();
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& queue_push_flag,
        std::atomic<bool>& stop,
        std::atomic<bool>& execute_done
    ) {
        if (!arm_controller_.initialize()) {
            Log::error("[ControlEnd::run] Failed to initialize actuators.");
            stop = true;
            return;
        }

        while (true) {
            std::unique_lock lock(queue_mutex);
            queue_push_flag.wait(lock, [&] {
                return !queue.empty() || stop;
            });

            if (!queue.empty()) {
                Bridge::Payload payload = queue.front();
                queue.pop();
                Log::debug(Log::magenta(std::format("[Queue] Queue has {} -> {} items. (popped)", queue.size() + 1, queue.size())));

                Log::debug(std::format("[ControlEnd::run] Received Payload with x: {}", payload.x));
                execute(payload);

                execute_done = true;
                Log::debug("execute_done = true;");

                lock.unlock();
            }
            else if (stop) {
                lock.unlock();

                Log::info("[ControlEnd::run] Stopping as stop signal received.");
                break;
            }
        }
    }
};

class VisionEndPnP {
private:
    Camera cam_top{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
    Camera cam_left{CameraType::LEFT, {1, cv::CAP_MSMF}, 90};
    Camera cam_right{CameraType::RIGHT, {2, cv::CAP_MSMF}, 90};
    Tracker tracker_top{TOP_ORANGE_MIN, TOP_ORANGE_MAX};
    Tracker tracker_left{LEFT_ORANGE_MIN, LEFT_ORANGE_MAX};
    Tracker tracker_right{RIGHT_ORANGE_MIN, RIGHT_ORANGE_MAX};
    Predictor predictor;

    cv::Mat latest_top_frame, latest_left_frame, latest_right_frame;
    std::mutex frame_mutex;

    std::atomic<bool> use_top_camera{true};
    bool awaiting_landing_push = false;
    bool awaiting_prediction = true;

public:
    explicit VisionEndPnP(const bool use_top_camera) : use_top_camera{use_top_camera} {
        cam_top.set_frame_callback([this](cv::Mat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_top << frame;
            if (const auto pos = tracker_top.get_camera_pos()) {
                predictor.set_point_top(pos.value().first);
                Draw::put_circle(frame, pos.value().first, pos.value().second, COLOR_GREEN);
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
                {10, 20}
            );
            frame.copyTo(latest_top_frame);
        });

        cam_left.set_frame_callback([this](cv::Mat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_left << frame;
            if (const auto pos = tracker_left.get_camera_pos()) {
                predictor.set_point_left(pos.value().first);
                Draw::put_circle(frame, pos.value().first, pos.value().second, COLOR_GREEN);
                // Log::debug(std::format("left found {}, {}", pos.value().first.x, pos.value().first.y));
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_left.get_fps(), cam_left.get_prop(cv::CAP_PROP_FPS)),
                {10, 20}
            );
            frame.copyTo(latest_left_frame);
        });

        cam_right.set_frame_callback([this](cv::Mat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_right << frame;
            if (const auto pos = tracker_right.get_camera_pos()) {
                predictor.set_point_right(pos.value().first);
                Draw::put_circle(frame, pos.value().first, pos.value().second, COLOR_GREEN);
                // Log::debug(std::format("right found {}, {}", pos.value().first.x, pos.value().first.y));
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_right.get_fps(), cam_right.get_prop(cv::CAP_PROP_FPS)),
                {10, 20}
            );
            frame.copyTo(latest_right_frame);
        });
    }

    ~VisionEndPnP() {
        cam_top.stop();
        cam_left.stop();
        cam_right.stop();
        cv::destroyAllWindows();
    }

    bool read_frame(cv::Mat& frame_top, cv::Mat& frame_left, cv::Mat& frame_right) {
        std::lock_guard lock(frame_mutex);

        latest_top_frame.copyTo(frame_top);
        latest_left_frame.copyTo(frame_left);
        latest_right_frame.copyTo(frame_right);

        if (frame_top.empty() || frame_left.empty() || frame_right.empty()) {
            // Log::warn("[VisionEnd::run] One or more frames are empty, skipping visualization.");
            return false;
        }

        return true;
    }

    void process_data(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& queue_push_flag,
        std::atomic<bool>& execute_done,
        // std::optional<cv::Point3f>& first_world_pos, // REMOVE
        cv::Point3f& world_pos,
        cv::Vec3f& world_speed,
        cv::Point3f& predict_init_pos,
        cv::Point3f& predict_arrive_pos,
        cv::Point3f& real_arrive_pos,
        std::vector<cv::Point3f>& orbit_3d,
        std::vector<cv::Point2f>& orbit_2d_top,
        std::vector<cv::Point2f>& orbit_2d_left,
        std::vector<cv::Point2f>& orbit_2d_right
    ) {
        const double fallback_dt = 1.0 / std::min({cam_top.get_fps(), cam_left.get_fps(), cam_right.get_fps()});
        if (const auto find_world_pos = predictor.get_new_world_pos(cam_top, cam_left, cam_right, static_cast<float>(fallback_dt),
                                                                    use_top_camera)) {
            world_pos = find_world_pos.value();

            if (world_pos.y >= PREDICT_MIN_Y) {
                awaiting_landing_push = true;
                Log::green("awaiting_landing_push = true;");
            }

            // Reset awaiting_prediction once the ball has passed below table Y-origin (new ball cycle)
            if (world_pos.y < 0) {
                awaiting_prediction = true;
            }

            orbit_3d.push_back(world_pos);
            orbit_2d_top.push_back(Predictor::pos_3d_to_2d(cam_top, world_pos));
            orbit_2d_left.push_back(Predictor::pos_3d_to_2d(cam_left, world_pos));
            orbit_2d_right.push_back(Predictor::pos_3d_to_2d(cam_right, world_pos));

            if (orbit_3d.size() > 100) {
                orbit_3d.erase(orbit_3d.begin());
                orbit_2d_top.erase(orbit_2d_top.begin());
                orbit_2d_left.erase(orbit_2d_left.begin());
                orbit_2d_right.erase(orbit_2d_right.begin());
            }
        }

        if (const auto find_world_speed = predictor.get_world_speed()) {
            world_speed = find_world_speed.value();
        }

        if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= launcher_pos.y && awaiting_prediction) {
            // Capture this world position for prediction
            awaiting_prediction = false;

            // Predict the trajectory and immediately push the action
            const auto p1 = launcher_pos;
            const auto p2 = world_pos;
            const auto dp = cv::Vec3f(p2 - p1);
            const auto p_dir = dp / cv::norm(dp);
            const auto init_speed = launch_velocity * p_dir;

            if (const auto t_arrive = Predictor::get_arrive_time(launcher_pos, init_speed)) {
                if (const auto find_prediction = Predictor::predict(launcher_pos, init_speed, t_arrive.value())) {
                    const auto& [init_pos, init_speed_val, pred_pos, pred_speed, duration] = find_prediction.value();
                    predict_init_pos = init_pos;
                    predict_arrive_pos = pred_pos;

                    Log::debug(std::format("[Prediction] {}, {} ==== {}s ===> {}, {}", Log::magenta(Log::to_string(init_pos)),
                                           Log::magenta(Log::to_string(init_speed_val)),
                                           duration, Log::blue(Log::to_string(pred_pos)), Log::blue(Log::to_string(pred_speed))));

                    // Immediately push to queue if ready
                    std::lock_guard lock(queue_mutex);
                    if (execute_done.load() && queue.empty()) {
                        Bridge::Payload payload = Bridge::actions(predict_arrive_pos);

                        execute_done = false;
                        Log::debug("execute_done = false;");

                        predict_arrive_pos = {-1, -1, -1};
                        world_pos = {-1, -1, -1};
                        world_speed = {0, 0, 0};

                        queue.push(payload);
                        queue_push_flag.notify_one();

                        Log::debug(Log::magenta(std::format("[Queue] Queue has {} -> {} items. (pushed)", queue.size() - 1, queue.size())));
                        Log::debug(std::format("[VisionEnd::run]: Payload set with x: {}", payload.x));
                    }
                }
            }
        }

        if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
            std::abs(world_pos.y) < 2 &&
            0 <= world_pos.z) {
            real_arrive_pos = world_pos;
        }
    }

    void visualize_data(
        cv::Mat& frame_top,
        cv::Mat& frame_left,
        cv::Mat& frame_right,
        const cv::Point3f& world_pos,
        const cv::Point3f& predict_init_pos,
        const cv::Point3f& predict_arrive_pos,
        const cv::Point3f& real_arrive_pos,
        const std::vector<cv::Point3f>& orbit,
        const std::vector<cv::Point2f>& orbit_2d_top,
        const std::vector<cv::Point2f>& orbit_2d_left,
        const std::vector<cv::Point2f>& orbit_2d_right
    ) const {
        // orbit
        for (std::size_t i = 0; i < orbit.size(); ++i) {
            Draw::put_circle(frame_top, orbit_2d_top[i], 3, COLOR_MAGENTA);
            Draw::put_circle(frame_left, orbit_2d_left[i], 3, COLOR_MAGENTA);
            Draw::put_circle(frame_right, orbit_2d_right[i], 3, COLOR_MAGENTA);
        }

        // predict init pos
        Draw::put_circle(frame_top, Predictor::pos_3d_to_2d(cam_top, predict_init_pos), 8, COLOR_MAGENTA);
        Draw::put_circle(frame_left, Predictor::pos_3d_to_2d(cam_left, predict_init_pos), 8, COLOR_MAGENTA);
        Draw::put_circle(frame_right, Predictor::pos_3d_to_2d(cam_right, predict_init_pos), 8, COLOR_MAGENTA);

        // predict arrive pos
        Draw::put_circle(frame_top, Predictor::pos_3d_to_2d(cam_top, predict_arrive_pos), 8, COLOR_BLUE);
        Draw::put_circle(frame_left, Predictor::pos_3d_to_2d(cam_left, predict_arrive_pos), 8, COLOR_BLUE);
        Draw::put_circle(frame_right, Predictor::pos_3d_to_2d(cam_right, predict_arrive_pos), 8, COLOR_BLUE);

        // real arrive pos
        Draw::put_circle(frame_top, Predictor::pos_3d_to_2d(cam_top, real_arrive_pos), 8, COLOR_RED);
        Draw::put_circle(frame_left, Predictor::pos_3d_to_2d(cam_left, real_arrive_pos), 8, COLOR_RED);
        Draw::put_circle(frame_right, Predictor::pos_3d_to_2d(cam_right, real_arrive_pos), 8, COLOR_RED);
    }

    void legend(
        cv::Mat& frame,
        const cv::Point3f& world_pos,
        const cv::Vec3f& world_speed,
        const cv::Point3f& predict_init_pos,
        const cv::Point3f& predict_arrive_pos,
        const cv::Point3f& real_arrive_pos
    ) {
        Draw::put_text_border(frame, Draw::to_string("Current Speed", world_speed, "cm/s"), {10, 40}, COLOR_CYAN);
        Draw::put_text_border(frame, Draw::to_string("Current Pos", world_pos, "cm"), {10, 60}, COLOR_GREEN);
        Draw::put_text_border(frame, Draw::to_string("Predict Init Pos", predict_init_pos, "cm/s"), {10, 80}, COLOR_MAGENTA);
        Draw::put_text_border(frame, Draw::to_string("Predict Arrive Pos", predict_arrive_pos, "cm"), {10, 100}, COLOR_BLUE);
        Draw::put_text_border(frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), {10, 120}, COLOR_RED);
    }

    cv::Mat combine_frame(const cv::Mat& frame_top, const cv::Mat& frame_left, const cv::Mat& frame_right) {
        cv::Mat frame_top_resized, frame_left_resized, frame_right_resized;
        cv::resize(frame_top, frame_top_resized, {}, 0.7, 0.7);
        cv::resize(frame_left, frame_left_resized, {}, 0.8, 0.8);
        cv::resize(frame_right, frame_right_resized, {}, 0.8, 0.8);

        const int max_width = std::max(frame_left_resized.cols + frame_right_resized.cols, frame_top_resized.cols);
        const int total_height = frame_top_resized.rows + std::max(frame_left_resized.rows, frame_right_resized.rows);
        const int top_x_offset = (max_width - frame_top_resized.cols) / 2;

        cv::Mat final_frame = cv::Mat::zeros(total_height, max_width, frame_left_resized.type());
        frame_top_resized.copyTo(final_frame(cv::Rect(top_x_offset, 0, frame_top_resized.cols, frame_top_resized.rows)));
        frame_left_resized.copyTo(final_frame(cv::Rect(0, frame_top_resized.rows, frame_left_resized.cols, frame_left_resized.rows)));
        frame_right_resized.copyTo(final_frame(cv::Rect(frame_left_resized.cols, frame_top_resized.rows, frame_right_resized.cols,
                                                        frame_right_resized.rows)));

        return final_frame;
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& queue_push_flag,
        std::atomic<bool>& stop,
        std::atomic<bool>& execute_done
    ) {
        if (!cam_top.is_opened() || !cam_left.is_opened() || !cam_right.is_opened()) {
            Log::error(std::format(
                "[VisionEnd::run] Failed to open cameras: top={}, left={}, right={}",
                cam_top.is_opened() ? "opened" : "failed",
                cam_left.is_opened() ? "opened" : "failed",
                cam_right.is_opened() ? "opened" : "failed"
            ));
            return;
        }

        cam_top.start();
        cam_left.start();
        cam_right.start();

        std::vector<cv::Point3f> orbit_3d;
        std::vector<cv::Point2f> orbit_2d_top;
        std::vector<cv::Point2f> orbit_2d_left;
        std::vector<cv::Point2f> orbit_2d_right;

        cv::Point3f world_pos{-1, -1, -1};
        // std::optional<cv::Point3f> first_world_pos = std::nullopt; // REMOVE
        cv::Vec3f world_speed{0, 0, 0};
        cv::Point3f predict_init_pos{-1, -1, -1};
        cv::Point3f predict_arrive_pos{-1, -1, -1};
        cv::Point3f real_arrive_pos{-1, -1, -1};

        while (true) {
            cv::Mat frame_top, frame_left, frame_right;
            if (!this->read_frame(frame_top, frame_left, frame_right)) continue;

            this->process_data(
                queue, queue_mutex, queue_push_flag, execute_done,
                // first_world_pos,
                world_pos, world_speed, predict_init_pos, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top, orbit_2d_left, orbit_2d_right
            );

            this->visualize_data(
                frame_top, frame_left, frame_right,
                world_pos, predict_init_pos, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top, orbit_2d_left, orbit_2d_right
            );

            cv::Mat frame_combined = this->combine_frame(frame_top, frame_left, frame_right);
            this->legend(frame_combined, world_pos, world_speed, predict_init_pos, predict_arrive_pos, real_arrive_pos);

            cv::imshow("Pingpong Robot / Press 'Q' to quit.", frame_combined);
            if (cv::waitKey(1) == 'q') {
                stop = true;
                queue_push_flag.notify_all();

                Log::info("[VisionEnd::run] Stopping as 'q' key pressed.");
                break;
            }
        }
    }
};

int main() {
    VisionEndPnP vision_end(true);
    ControlEnd control_end;

    std::queue<Bridge::Payload> queue;
    std::mutex queue_mutex;
    std::condition_variable flag;
    std::atomic stop = false;
    std::atomic execute_done = true;

    auto producer = std::thread(
        &VisionEndPnP::run, &vision_end,
        std::ref(queue),
        std::ref(queue_mutex),
        std::ref(flag),
        std::ref(stop),
        std::ref(execute_done)
    );

    auto consumer = std::thread(
        &ControlEnd::run, &control_end,
        std::ref(queue),
        std::ref(queue_mutex),
        std::ref(flag),
        std::ref(stop),
        std::ref(execute_done)
    );

    producer.join();
    consumer.join();

    return 0;
}
