#include <vector>
#include <opencv2/opencv.hpp>
#include "utils/draw.hpp"
#include "vision/camera.hpp"
#include "vision/tracker.hpp"
#include "vision/predictor.hpp"
#include "vision/bridge.hpp"

// TODO: 카메라에 공이 적어도 한 대라도 안 보일 경우 위치가 겁나 튀는 문제 해결 필요 - 이 때 kalman filter 같은 걸로 예측?
// TODO: Kalman filter 적용 로직 구현 필요
// TODO: 카메라 동기화 로직 구현 필요

class VisionEndPnP {
    // TODO: top을 쓰는 것과 안 쓰는 것의 정확도 비교하기

private:
    Camera cam_top{ CameraType::TOP, {0, cv::CAP_MSMF}, 90 };
    Camera cam_left{ CameraType::LEFT, {1, cv::CAP_MSMF}, 90 };
    Camera cam_right{ CameraType::RIGHT, {2, cv::CAP_MSMF}, 90 };
    Tracker tracker_top{ ORANGE_MIN, ORANGE_MAX };
    Tracker tracker_left{ ORANGE_MIN, ORANGE_MAX };
    Tracker tracker_right{ ORANGE_MIN, ORANGE_MAX };
    Predictor predictor;

    cv::UMat latest_top_frame, latest_left_frame, latest_right_frame;
    std::mutex frame_mutex;

    std::atomic<bool> use_top_camera{ true };

public:
    explicit VisionEndPnP(const bool use_top_camera) : use_top_camera{ use_top_camera } {
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
                { 10, 20 }
            );
            frame.copyTo(latest_top_frame);
            });

        cam_left.set_frame_callback([this](cv::UMat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_left << frame;
            if (const auto pos = tracker_left.get_camera_pos()) {
                predictor.set_point_left(pos.value().first);
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_left.get_fps(), cam_left.get_prop(cv::CAP_PROP_FPS)),
                { 10, 20 }
            );
            frame.copyTo(latest_left_frame);
            });

        cam_right.set_frame_callback([this](cv::UMat& frame) {
            if (frame.empty()) return;

            std::lock_guard lock(frame_mutex);
            tracker_right << frame;
            if (const auto pos = tracker_right.get_camera_pos()) {
                predictor.set_point_right(pos.value().first);
            }

            Draw::put_text_border(
                frame,
                std::format("FPS: {:.2f}/{:.2f}", cam_right.get_fps(), cam_right.get_prop(cv::CAP_PROP_FPS)),
                { 10, 20 }
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

    void read_frame(cv::UMat& frame_top, cv::UMat& frame_left, cv::UMat& frame_right) {
        std::lock_guard lock(frame_mutex);
        if (latest_top_frame.empty() || latest_left_frame.empty() || latest_right_frame.empty())
            return;

        latest_top_frame.copyTo(frame_top);
        latest_left_frame.copyTo(frame_left);
        latest_right_frame.copyTo(frame_right);
    }

    void process_data(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& flag,
        cv::Point3f& world_pos,
        cv::Vec3f& world_speed,
        cv::Point3f& predict_arrive_pos,
        cv::Point3f& real_arrive_pos,
        std::vector<cv::Point3f> orbit_3d,
        std::vector<cv::Point2f> orbit_2d_top,
        std::vector<cv::Point2f> orbit_2d_left,
        std::vector<cv::Point2f> orbit_2d_right
    ) {
        const double fallback_dt = 1.0 / std::min({ cam_top.get_fps(), cam_left.get_fps(), cam_right.get_fps() });
        if (const auto find_world_pos = predictor.get_new_world_pos(cam_top, cam_left, cam_right, static_cast<float>(fallback_dt),
            use_top_camera)) {
            world_pos = find_world_pos.value();

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
                // TODO: 마지막 도착 직전 속도 예측해서 넣기 - 회귀 식에서 같이 리턴하기
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
        const std::vector<cv::Point3f>& orbit,
        const std::vector<cv::Point2f>& orbit_2d_top,
        const std::vector<cv::Point2f>& orbit_2d_left,
        const std::vector<cv::Point2f>& orbit_2d_right
    ) const {
        for (std::size_t i = 0; i < orbit.size(); ++i) {
            Draw::put_circle(frame_top, orbit_2d_top[i], 3, COLOR_MAGENTA);
            Draw::put_circle(frame_left, orbit_2d_left[i], 3, COLOR_MAGENTA);
            Draw::put_circle(frame_right, orbit_2d_right[i], 3, COLOR_MAGENTA);
        }

        const auto world_pos_2d_top = Predictor::pos_3d_to_2d(cam_top, world_pos);
        const auto world_pos_2d_left = Predictor::pos_3d_to_2d(cam_left, world_pos);
        const auto world_pos_2d_right = Predictor::pos_3d_to_2d(cam_right, world_pos);

        Draw::put_circle(frame_top, world_pos_2d_top, 8, COLOR_GREEN);
        Draw::put_circle(frame_left, world_pos_2d_left, 8, COLOR_GREEN);
        Draw::put_circle(frame_right, world_pos_2d_right, 8, COLOR_GREEN);

        const auto world_speed_2d_top = Predictor::pos_3d_to_2d(cam_top, world_speed);
        const auto world_speed_2d_left = Predictor::pos_3d_to_2d(cam_left, world_speed);
        const auto world_speed_2d_right = Predictor::pos_3d_to_2d(cam_right, world_speed);

        Draw::put_arrow(frame_top, world_pos_2d_top, world_pos_2d_top + world_speed_2d_top, COLOR_GREEN);
        Draw::put_arrow(frame_left, world_pos_2d_left, world_pos_2d_left + world_speed_2d_left, COLOR_GREEN);
        Draw::put_arrow(frame_right, world_pos_2d_right, world_pos_2d_right + world_speed_2d_right, COLOR_GREEN);

        Draw::put_circle(frame_top, Predictor::pos_3d_to_2d(cam_top, predict_arrive_pos), 8, COLOR_BLUE);
        Draw::put_circle(frame_left, Predictor::pos_3d_to_2d(cam_left, predict_arrive_pos), 8, COLOR_BLUE);
        Draw::put_circle(frame_right, Predictor::pos_3d_to_2d(cam_right, predict_arrive_pos), 8, COLOR_BLUE);

        Draw::put_circle(frame_top, Predictor::pos_3d_to_2d(cam_top, real_arrive_pos), 8, COLOR_RED);
        Draw::put_circle(frame_left, Predictor::pos_3d_to_2d(cam_left, real_arrive_pos), 8, COLOR_RED);
        Draw::put_circle(frame_right, Predictor::pos_3d_to_2d(cam_right, real_arrive_pos), 8, COLOR_RED);
    }

    void legend(
        cv::UMat& frame,
        const cv::Point3f& world_pos,
        const cv::Vec3f& world_speed,
        const cv::Point3f& predict_arrive_pos,
        const cv::Point3f& real_arrive_pos
    ) const {
        Draw::put_text_border(frame, Draw::to_string("World Pos", world_pos, "cm"), { 10, 50 }, COLOR_GREEN);
        Draw::put_text_border(frame, Draw::to_string("World Speed", world_speed, "cm/s"), { 10, 80 }, COLOR_CYAN);
        Draw::put_text_border(frame, Draw::to_string("Predict Pos", predict_arrive_pos, "cm"), { 10, 110 }, COLOR_BLUE);
        Draw::put_text_border(frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), { 10, 140 }, COLOR_RED);
    }

    cv::UMat& combine_frame(const cv::UMat& frame_top, const cv::UMat& frame_left, const cv::UMat& frame_right) {
        cv::UMat frame_top_resized, frame_left_resized, frame_right_resized;
        cv::resize(frame_top, frame_top_resized, {}, 0.7, 0.7);
        cv::resize(frame_left, frame_left_resized, {}, 0.8, 0.8);
        cv::resize(frame_right, frame_right_resized, {}, 0.8, 0.8);

        const int max_width = std::max(frame_left_resized.cols + frame_right_resized.cols, frame_top_resized.cols);
        const int total_height = frame_top_resized.rows + std::max(frame_left_resized.rows, frame_right_resized.rows);
        const int top_x_offset = (max_width - frame_top_resized.cols) / 2;

        cv::UMat final_frame = cv::UMat::zeros(total_height, max_width, frame_left_resized.type());
        frame_top_resized.copyTo(final_frame(cv::Rect(top_x_offset, 0, frame_top_resized.cols, frame_top_resized.rows)));
        frame_left_resized.copyTo(final_frame(cv::Rect(0, frame_top_resized.rows, frame_left_resized.cols, frame_left_resized.rows)));
        frame_right_resized.copyTo(final_frame(cv::Rect(frame_left_resized.cols, frame_top_resized.rows, frame_right_resized.cols,
            frame_right_resized.rows)));

        return final_frame;
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& flag,
        std::atomic<bool>& stop
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

        cv::Point3f world_pos{ -1, -1, -1 };
        cv::Vec3f world_speed{ 0, 0, 0 };
        cv::Point3f predict_arrive_pos{ -1, -1, -1 };
        cv::Point3f real_arrive_pos{ -1, -1, -1 };
        std::vector<cv::Point3f> orbit_3d;
        std::vector<cv::Point2f> orbit_2d_top;
        std::vector<cv::Point2f> orbit_2d_left;
        std::vector<cv::Point2f> orbit_2d_right;

        while (true) {
            this->process_data(
                queue, queue_mutex, flag, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top, orbit_2d_left, orbit_2d_right
            );

            cv::UMat frame_top, frame_left, frame_right;
            this->read_frame(frame_top, frame_left, frame_right);
            this->visualize_data(
                frame_top, frame_left, frame_right, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
                orbit_3d, orbit_2d_top, orbit_2d_left, orbit_2d_right
            );

            cv::UMat frame_combined = this->combine_frame(frame_top, frame_left, frame_right);
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
                fs << "orbit_2d_left" << orbit_2d_left;
                fs << "orbit_2d_right" << orbit_2d_right;
                fs.release();
            }
        }
    }
};

// class VisionEndStereo {
//     // TODO: top stereo or realsense to get depth
//
// private:
//     Camera cam_top{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
//     // Camera cam_top_2{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
//     Tracker tracker_top{ORANGE_MIN, ORANGE_MAX};
//     // Tracker tracker_top_2{ORANGE_MIN, ORANGE_MAX};
//     Predictor predictor;
//
//     cv::UMat latest_top_frame;
//     // cv::UMat latest_top_frame_2;
//     std::mutex frame_mutex;
//
// public:
//     VisionEndStereo() {
//         cam_top.set_frame_callback([this](cv::UMat& frame) {
//             if (frame.empty()) return;
//
//             std::lock_guard lock(frame_mutex);
//             tracker_top << frame;
//             if (const auto pos = tracker_top.get_camera_pos()) {
//                 predictor.set_point_top(pos.value().first);
//             }
//
//             Draw::put_text_border(
//                 frame,
//                 std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
//                 {10, 20}
//             );
//             frame.copyTo(latest_top_frame);
//         });
//     }
//
//     ~VisionEndStereo() {
//         cam_top.stop();
//         cv::destroyAllWindows();
//     }
//
//     void read_frame(cv::UMat& frame_top) {
//         std::lock_guard lock(frame_mutex);
//         if (latest_top_frame.empty())
//             return;
//
//         latest_top_frame.copyTo(frame_top);
//     }
//
//     void process_data(
//         std::queue<Bridge::Payload>& queue,
//         std::mutex& queue_mutex,
//         std::condition_variable& flag,
//         cv::Point3f& world_pos,
//         cv::Vec3f& world_speed,
//         cv::Point3f& predict_arrive_pos,
//         cv::Point3f& real_arrive_pos,
//         std::deque<cv::Point3f> orbit_3d,
//         std::deque<cv::Point2f> orbit_2d_top
//     ) {
//         // TODO: stereo calibration으로 실제 위치 구하기 or realsense depth map
//
//         const double fallback_dt = 1.0 / std::min({cam_top.get_fps()});
//         if (const auto find_world_pos = predictor.get_new_world_pos(static_cast<float>(fallback_dt))) {
//             world_pos = find_world_pos.value();
//
//             orbit_3d.push_back(world_pos);
//             orbit_2d_top.push_back(predictor.pos_3d_to_2d(world_pos, CameraType::TOP));
//             if (orbit_3d.size() > 100) {
//                 orbit_3d.pop_front();
//                 orbit_2d_top.pop_front();
//             }
//         }
//
//         if (const auto find_world_speed = predictor.get_world_speed()) {
//             world_speed = find_world_speed.value();
//         }
//
//         if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= PREDICT_MAX_Y) {
//             if (const auto find_predict_pos = predictor.get_arrive_pos()) {
//                 predict_arrive_pos = find_predict_pos.value();
//             }
//         }
//         else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
//             0 <= world_pos.y && world_pos.y < PREDICT_MIN_Y &&
//             0 <= world_pos.z
//         ) {
//             std::lock_guard lock(queue_mutex);
//             if (queue.empty()) {
//                 // TODO: 마지막 도착 직전 속도 예측해서 넣기 - 회귀 식에서 같이 리턴하기
//                 Bridge::Payload payload = Bridge::convert(predict_arrive_pos, world_speed);
//                 queue.push(payload);
//                 flag.notify_one();
//
//                 Log::debug(std::format(
//                     "[VisionEnd::run]: Payload set with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
//                     payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
//             }
//         }
//
//         if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
//             std::abs(world_pos.y) < 5 &&
//             0 <= world_pos.z) {
//             real_arrive_pos = world_pos;
//         }
//     }
//
//     void visualize_data(
//         cv::UMat& frame_top,
//         const cv::Point3f& world_pos,
//         const cv::Vec3f& world_speed,
//         const cv::Point3f& predict_arrive_pos,
//         const cv::Point3f& real_arrive_pos,
//         const std::deque<cv::Point3f>& orbit,
//         const std::deque<cv::Point2f>& orbit_2d_top
//     ) const {
//         for (std::size_t i = 0; i < orbit.size(); ++i) {
//             Draw::put_circle(frame_top, orbit_2d_top[i], 3, COLOR_MAGENTA);
//         }
//
//         const auto world_pos_2d_top = predictor.pos_3d_to_2d(world_pos, CameraType::TOP);
//
//         Draw::put_circle(frame_top, world_pos_2d_top, 8, COLOR_GREEN);
//
//         const auto world_speed_2d_top = predictor.pos_3d_to_2d(world_speed, CameraType::TOP);
//
//         Draw::put_arrow(frame_top, world_pos_2d_top, world_pos_2d_top + world_speed_2d_top, COLOR_GREEN);
//
//         Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::TOP), 8, COLOR_BLUE);
//
//         Draw::put_circle(frame_top, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP), 8, COLOR_RED);
//     }
//
//     void legend(
//         cv::UMat& frame,
//         const cv::Point3f& world_pos,
//         const cv::Vec3f& world_speed,
//         const cv::Point3f& predict_arrive_pos,
//         const cv::Point3f& real_arrive_pos
//     ) const {
//         Draw::put_text_border(frame, Draw::to_string("World Pos", world_pos, "cm"), {10, 50}, COLOR_GREEN);
//         Draw::put_text_border(frame, Draw::to_string("World Speed", world_speed, "cm/s"), {10, 80}, COLOR_CYAN);
//         Draw::put_text_border(frame, Draw::to_string("Predict Pos", predict_arrive_pos, "cm"), {10, 110}, COLOR_BLUE);
//         Draw::put_text_border(frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), {10, 140}, COLOR_RED);
//     }
//
//     cv::UMat& combine_frame(const cv::UMat& frame_top_1, const cv::UMat& frame_top_2) {
//         cv::UMat concatenated_frame;
//         cv::hconcat(frame_top_1, frame_top_2, concatenated_frame);
//         cv::resize(concatenated_frame, concatenated_frame, {}, 0.7, 0.7);
//
//         return concatenated_frame;
//     }
//
//     void run(
//         std::queue<Bridge::Payload>& queue,
//         std::mutex& queue_mutex,
//         std::condition_variable& flag,
//         std::atomic<bool>& stop
//     ) {
//         if (!cam_top.is_opened()) {
//             Log::error(std::format(
//                 "[VisionEnd::run] Failed to open cameras: top={}",
//                 cam_top.is_opened() ? "opened" : "failed"
//             ));
//             return;
//         }
//
//         cam_top.start();
//
//         cv::Point3f world_pos{-1, -1, -1};
//         cv::Vec3f world_speed{0, 0, 0};
//         cv::Point3f predict_arrive_pos{-1, -1, -1};
//         cv::Point3f real_arrive_pos{-1, -1, -1};
//         std::deque<cv::Point3f> orbit_3d;
//         std::deque<cv::Point2f> orbit_2d_top;
//
//         while (true) {
//             this->process_data(
//                 queue, queue_mutex, flag, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
//                 orbit_3d, orbit_2d_top
//             );
//
//             cv::UMat frame_top;
//             this->read_frame(frame_top);
//             this->visualize_data(frame_top, world_pos, world_speed, predict_arrive_pos, real_arrive_pos, orbit_3d, orbit_2d_top);
//
//             cv::UMat frame_combined = frame_top; // this->combine_frame(frame_top, frame_top_2);
//             this->legend(frame_combined, world_pos, world_speed, predict_arrive_pos, real_arrive_pos);
//
//             cv::imshow("Pingpong Robot / Press 'Q' to quit, 'S' to save orbit.", frame_combined);
//
//             const int key = cv::waitKey(1);
//             if (key == 'q') {
//                 stop = true;
//                 flag.notify_all();
//                 Log::info("[VisionEnd::run] Stopping as 'q' key pressed.");
//
//                 break;
//             }
//             else if (key == 's') {
//                 cv::FileStorage fs(PATH_ORBIT, cv::FileStorage::WRITE);
//                 if (!fs.isOpened()) {
//                     Log::error("[VisionEnd::run] Failed to open file for saving orbit.");
//                 }
//
//                 fs << "orbit_3d" << orbit_3d;
//                 fs << "orbit_2d_top" << orbit_2d_top;
//                 fs.release();
//             }
//         }
//     }
// };
//
// class VisionEndIgnoreZ {
// private:
//     Camera cam_top{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
//     // Camera cam_top_2{CameraType::TOP, {0, cv::CAP_MSMF}, 90};
//     Tracker tracker_top{ORANGE_MIN, ORANGE_MAX};
//     // Tracker tracker_top_2{ORANGE_MIN, ORANGE_MAX};
//     Predictor predictor;
//
//     cv::UMat latest_top_frame;
//     // cv::UMat latest_top_frame_2;
//     std::mutex frame_mutex;
//
// public:
//     VisionEndIgnoreZ() {
//         cam_top.set_frame_callback([this](cv::UMat& frame) {
//             if (frame.empty()) return;
//
//             std::lock_guard lock(frame_mutex);
//             tracker_top << frame;
//             if (const auto pos = tracker_top.get_camera_pos()) {
//                 predictor.set_point_top(pos.value().first);
//             }
//
//             Draw::put_text_border(
//                 frame,
//                 std::format("FPS: {:.2f}/{:.2f}", cam_top.get_fps(), cam_top.get_prop(cv::CAP_PROP_FPS)),
//                 {10, 20}
//             );
//             frame.copyTo(latest_top_frame);
//         });
//     }
//
//     ~VisionEndIgnoreZ() {
//         cam_top.stop();
//         cv::destroyAllWindows();
//     }
//
//     void read_frame(cv::UMat& frame_top) {
//         std::lock_guard lock(frame_mutex);
//         if (latest_top_frame.empty())
//             return;
//
//         latest_top_frame.copyTo(frame_top);
//     }
//
//     void process_data(
//         std::queue<Bridge::Payload>& queue,
//         std::mutex& queue_mutex,
//         std::condition_variable& flag,
//         cv::Point3f& world_pos,
//         cv::Vec3f& world_speed,
//         cv::Point3f& predict_arrive_pos,
//         cv::Point3f& real_arrive_pos,
//         std::deque<cv::Point3f> orbit_3d,
//         std::deque<cv::Point2f> orbit_2d_top
//     ) {
//         // TODO: 탁구대의 왼쪽 끝과 오른쪽 끝의 camera point, world point 값을 구해서 비례식으로 실제 위치 구하기
//
//         const double fallback_dt = 1.0 / std::min({cam_top.get_fps()});
//         if (const auto find_world_pos = predictor.get_new_world_pos(static_cast<float>(fallback_dt))) {
//             world_pos = find_world_pos.value();
//
//             orbit_3d.push_back(world_pos);
//             orbit_2d_top.push_back(predictor.pos_3d_to_2d(world_pos, CameraType::TOP));
//             if (orbit_3d.size() > 100) {
//                 orbit_3d.pop_front();
//                 orbit_2d_top.pop_front();
//             }
//         }
//
//         if (const auto find_world_speed = predictor.get_world_speed()) {
//             world_speed = find_world_speed.value();
//         }
//
//         if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= PREDICT_MAX_Y) {
//             if (const auto find_predict_pos = predictor.get_arrive_pos()) {
//                 predict_arrive_pos = find_predict_pos.value();
//             }
//         }
//         else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
//             0 <= world_pos.y && world_pos.y < PREDICT_MIN_Y &&
//             0 <= world_pos.z
//         ) {
//             std::lock_guard lock(queue_mutex);
//             if (queue.empty()) {
//                 // TODO: 마지막 도착 직전 속도 예측해서 넣기 - 회귀 식에서 같이 리턴하기
//                 Bridge::Payload payload = Bridge::convert(predict_arrive_pos, world_speed);
//                 queue.push(payload);
//                 flag.notify_one();
//
//                 Log::debug(std::format(
//                     "[VisionEnd::run]: Payload set with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
//                     payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
//             }
//         }
//
//         if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
//             std::abs(world_pos.y) < 5 &&
//             0 <= world_pos.z) {
//             real_arrive_pos = world_pos;
//         }
//     }
//
//     void visualize_data(
//         cv::UMat& frame_top,
//         const cv::Point3f& world_pos,
//         const cv::Vec3f& world_speed,
//         const cv::Point3f& predict_arrive_pos,
//         const cv::Point3f& real_arrive_pos,
//         const std::deque<cv::Point3f>& orbit,
//         const std::deque<cv::Point2f>& orbit_2d_top
//     ) const {
//         for (std::size_t i = 0; i < orbit.size(); ++i) {
//             Draw::put_circle(frame_top, orbit_2d_top[i], 3, COLOR_MAGENTA);
//         }
//
//         const auto world_pos_2d_top = predictor.pos_3d_to_2d(world_pos, CameraType::TOP);
//
//         Draw::put_circle(frame_top, world_pos_2d_top, 8, COLOR_GREEN);
//
//         const auto world_speed_2d_top = predictor.pos_3d_to_2d(world_speed, CameraType::TOP);
//
//         Draw::put_arrow(frame_top, world_pos_2d_top, world_pos_2d_top + world_speed_2d_top, COLOR_GREEN);
//
//         Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::TOP), 8, COLOR_BLUE);
//
//         Draw::put_circle(frame_top, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP), 8, COLOR_RED);
//     }
//
//     void legend(
//         cv::UMat& frame,
//         const cv::Point3f& world_pos,
//         const cv::Vec3f& world_speed,
//         const cv::Point3f& predict_arrive_pos,
//         const cv::Point3f& real_arrive_pos
//     ) const {
//         Draw::put_text_border(frame, Draw::to_string("World Pos", world_pos, "cm"), {10, 50}, COLOR_GREEN);
//         Draw::put_text_border(frame, Draw::to_string("World Speed", world_speed, "cm/s"), {10, 80}, COLOR_CYAN);
//         Draw::put_text_border(frame, Draw::to_string("Predict Pos", predict_arrive_pos, "cm"), {10, 110}, COLOR_BLUE);
//         Draw::put_text_border(frame, Draw::to_string("Real Arrive Pos", real_arrive_pos, "cm"), {10, 140}, COLOR_RED);
//     }
//
//     cv::UMat& resize_frame(const cv::UMat& frame_top) {
//         cv::UMat frame_resized;
//         cv::resize(frame_top, frame_resized, {}, 0.7, 0.7);
//
//         return frame_resized;
//     }
//
//     void run(
//         std::queue<Bridge::Payload>& queue,
//         std::mutex& queue_mutex,
//         std::condition_variable& flag,
//         std::atomic<bool>& stop
//     ) {
//         if (!cam_top.is_opened()) {
//             Log::error(std::format(
//                 "[VisionEnd::run] Failed to open cameras: top={}",
//                 cam_top.is_opened() ? "opened" : "failed"
//             ));
//             return;
//         }
//
//         cam_top.start();
//
//         cv::Point3f world_pos{-1, -1, -1};
//         cv::Vec3f world_speed{0, 0, 0};
//         cv::Point3f predict_arrive_pos{-1, -1, -1};
//         cv::Point3f real_arrive_pos{-1, -1, -1};
//         std::deque<cv::Point3f> orbit_3d;
//         std::deque<cv::Point2f> orbit_2d_top;
//
//         while (true) {
//             this->process_data(
//                 queue, queue_mutex, flag, world_pos, world_speed, predict_arrive_pos, real_arrive_pos,
//                 orbit_3d, orbit_2d_top
//             );
//
//             cv::UMat frame_top;
//             this->read_frame(frame_top);
//             this->visualize_data(frame_top, world_pos, world_speed, predict_arrive_pos, real_arrive_pos, orbit_3d, orbit_2d_top);
//
//             cv::UMat frame = this->resize_frame(frame_top);
//             this->legend(frame, world_pos, world_speed, predict_arrive_pos, real_arrive_pos);
//
//             cv::imshow("Pingpong Robot / Press 'Q' to quit, 'S' to save orbit.", frame);
//
//             const int key = cv::waitKey(1);
//             if (key == 'q') {
//                 stop = true;
//                 flag.notify_all();
//                 Log::info("[VisionEnd::run] Stopping as 'q' key pressed.");
//
//                 break;
//             }
//             else if (key == 's') {
//                 cv::FileStorage fs(PATH_ORBIT, cv::FileStorage::WRITE);
//                 if (!fs.isOpened()) {
//                     Log::error("[VisionEnd::run] Failed to open file for saving orbit.");
//                 }
//
//                 fs << "orbit_3d" << orbit_3d;
//                 fs << "orbit_2d_top" << orbit_2d_top;
//                 fs.release();
//             }
//         }
//     }
// };

int main() {
    std::queue<Bridge::Payload> queue;
    std::mutex queue_mutex;
    std::condition_variable flag;
    std::atomic stop = false;

    VisionEndPnP vision_end_pnp(true);
    // VisionEndStereo vision_end_stereo;
    // VisionEndIgnoreZ vision_end_ignore_z;

    auto vision_thread_pnp = std::thread(&VisionEndPnP::run, &vision_end_pnp, std::ref(queue), std::ref(queue_mutex), std::ref(flag),
        std::ref(stop));
    // auto vision_thread_stereo = std::thread(&VisionEndStereo::run, &vision_end_stereo, std::ref(queue), std::ref(queue_mutex), std::ref(flag), std::ref(stop));
    // auto vision_thread_ignore_z = std::thread(&VisionEndIgnoreZ::run, &vision_end_ignore_z, std::ref(queue), std::ref(queue_mutex), std::ref(flag), std::ref(stop));

    vision_thread_pnp.join();
    // vision_thread_stereo.join();
    // vision_thread_ignore_z.join();

    return 0;
}
