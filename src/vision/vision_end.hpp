//
// Created by Hyunseung Ryu on 2025. 7. 30..
//

#ifndef VISION_END_HPP
#define VISION_END_HPP

#include <opencv2/opencv.hpp>
#include <vector>

#include "bridge.hpp"
#include "../utils/draw.hpp"
#include "camera.hpp"
#include "camera_type.hpp"
#include "predictor.hpp"
#include "tracker.hpp"
#include "visualizer.hpp"

#define DEBUG
#define IMSHOW

inline std::mutex mutex;
inline std::atomic has_sent{false};
inline std::optional<BridgePayload> shared_payload;

class VisionEnd {
private:
    Camera cam_top = Camera(CameraType::TOP, {0, 1200}, 120);
    Camera cam_left = Camera(CameraType::LEFT, {1, 1200}, 120);
    Camera cam_right = Camera(CameraType::RIGHT, {2, 1200}, 120);
    Tracker tracker_top = Tracker(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_left = Tracker(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_right = Tracker(ORANGE_MIN, ORANGE_MAX);
    Predictor predictor;

    static void frame_callback(
        cv::Mat& frame,
        const Camera& camera,
        Tracker& tracker,
        Predictor& predictor,
        void (Predictor::*set_pt)(const std::optional<cv::Point2f>&)
    ) {
        if (frame.empty()) return;
        tracker << frame;

        const auto camera_pos = tracker.get_camera_pos();
        (predictor.*set_pt)(camera_pos.has_value() ? camera_pos.value().first : std::nullopt);

#ifdef DEBUG
        Draw::put_text(
            frame,
            std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS)),
            {10, 20}
        );
#endif
    }

public:
    VisionEnd() {
        cam_top.set_frame_callback([this](cv::Mat& frame) {
            frame_callback(frame, cam_top, tracker_top, predictor, &Predictor::set_point_top);
        });
        cam_left.set_frame_callback([this](cv::Mat& frame) {
            frame_callback(frame, cam_left, tracker_left, predictor, &Predictor::set_point_left);
        });
        cam_right.set_frame_callback([this](cv::Mat& frame) {
            frame_callback(frame, cam_right, tracker_right, predictor, &Predictor::set_point_right);
        });
    }

    ~VisionEnd() {
        cam_top.stop();
        cam_left.stop();
        cam_right.stop();
        cv::destroyAllWindows();
    }

    void run() {
        // TODO: 카메라 레이턴시, 타임라인 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

        if (!cam_top.is_opened() || !cam_left.is_opened() || !cam_right.is_opened()) {
            const std::string message = std::format(
                "Failed to open camera: top={}, left={}, right={}",
                cam_top.is_opened() ? "opened" : "fail",
                cam_left.is_opened() ? "opened" : "fail",
                cam_right.is_opened() ? "opened" : "fail"
            );
            std::cerr << message << std::endl;
            return;
        }

        cam_top.start();
        cam_left.start();
        cam_right.start();

        cv::Point3f world_pos{-1, -1, -1};
        cv::Vec3f world_speed{0, 0, 0};
        cv::Point3f predict_arrive_pos{-1, -1, -1};
        cv::Point3f real_arrive_pos{-1, -1, -1};
        std::deque<cv::Point3f> orbit;
        std::deque<cv::Point3f> predict_orbit;

        while (true) {
            cv::Mat frame_top, frame_left, frame_right;
            cam_top >> frame_top;
            cam_left >> frame_left;
            cam_right >> frame_right;

            if (frame_top.empty() || frame_left.empty() || frame_right.empty())
                continue;

            // (4) 3D 위치 삼각측량
            if (const auto new_world_pos = predictor.get_new_world_pos(1.0f / static_cast<float>(cam_top.get_fps()))) {
                world_pos = new_world_pos.value();

#ifdef DEBUG
                orbit.push_back(world_pos);
                if (orbit.size() > 100) {
                    orbit.pop_front(); // 궤적이 너무 길어지지 않도록 제한
                }
#endif
            }

            if (const auto new_world_speed = predictor.get_world_speed()) {
                world_speed = new_world_speed.value();
            }

            // 예측 구간 설정
            if (PREDICT_MIN_Y <= world_pos.y && world_pos.y <= PREDICT_MAX_Y) {
                if (const auto new_arrive_pos = predictor.get_arrive_pos()) {
                    predict_arrive_pos = new_arrive_pos.value();
                }
            }
            else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
                0 <= world_pos.y &&
                0 <= world_pos.z &&
                !has_sent.load()) {
                {
                    std::lock_guard lock(mutex);
                    const auto payload = Bridge::convert(predict_arrive_pos);

                    std::cout << "Predicted position: " << predict_arrive_pos << std::endl;
                    std::cout << "Broadcast to hardware: " << std::endl
                        << "\tx: " << payload.x << " cm" << std::endl
                        << "\ttheta: " << payload.theta << " deg" << std::endl
                        << "\tswing_length: " << payload.swing_length << " deg" << std::endl
                        << "\twrist_angle: " << payload.wrist_angle << " deg" << std::endl;

                    shared_payload = payload;
                    has_sent = true;
                }
            }

#ifdef DEBUG
            // 실제 도착 위치 저장
            if (std::abs(world_pos.y - TABLE_HEIGHT) < 5) {
                real_arrive_pos = world_pos;
            }
#endif

#ifdef DEBUG
            Draw::put_text_border(
                frame_left,
                Draw::to_string("pos", world_pos, "cm"),
                {10, 50},
                COLOR_GREEN
            );

            Draw::put_text_border(
                frame_left,
                Draw::to_string("speed", world_speed, "cm/s"),
                {10, 80}
            );

            // 예상 도착 위치 시각화
            Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::TOP), 10, COLOR_BLUE);
            Draw::put_circle(frame_left, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::LEFT), 10, COLOR_BLUE);
            Draw::put_circle(frame_right, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::RIGHT), 10,
                             COLOR_BLUE);
            Draw::put_text_border(
                frame_left,
                Draw::to_string("predict", predict_arrive_pos, "cm"),
                {10, 110},
                COLOR_BLUE
            );

            // 실제 도착 위치 시각화
            Draw::put_circle(frame_top, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::TOP), 10, COLOR_RED);
            Draw::put_circle(frame_left, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::LEFT), 10, COLOR_RED);
            Draw::put_circle(frame_right, predictor.pos_3d_to_2d(real_arrive_pos, CameraType::RIGHT), 10, COLOR_RED);
            Draw::put_text(
                frame_left,
                Draw::to_string("real", real_arrive_pos, "cm"),
                {10, 140},
                COLOR_RED
            );

            // 궤적 그리기
            for (const auto& pos : orbit) {
                Draw::put_circle(frame_top, predictor.pos_3d_to_2d(pos, CameraType::TOP), 3, COLOR_MAGENTA);
                Draw::put_circle(frame_left, predictor.pos_3d_to_2d(pos, CameraType::LEFT), 3, COLOR_MAGENTA);
                Draw::put_circle(frame_right, predictor.pos_3d_to_2d(pos, CameraType::RIGHT), 3, COLOR_MAGENTA);
            }

            // 예상 궤적 그리기
            for (const auto& pos : predict_orbit) {
                Draw::put_circle(frame_top, predictor.pos_3d_to_2d(pos, CameraType::TOP), 3, COLOR_CYAN);
                Draw::put_circle(frame_left, predictor.pos_3d_to_2d(pos, CameraType::LEFT), 3, COLOR_CYAN);
                Draw::put_circle(frame_right, predictor.pos_3d_to_2d(pos, CameraType::RIGHT), 3, COLOR_CYAN);
            }
#endif

#ifdef IMSHOW
            cv::Mat frame_top_resized, frame_left_resized, frame_right_resized;
            cv::resize(frame_top, frame_top_resized, {}, 0.5, 0.5);
            cv::resize(frame_left, frame_left_resized, {}, 0.5, 0.5);
            cv::resize(frame_right, frame_right_resized, {}, 0.5, 0.5);

            cv::imshow("Top", frame_top);
            cv::imshow("Left", frame_left_resized);
            cv::imshow("Right", frame_right_resized);

            if (cv::waitKey(1) == 'q') break;
#endif
        }

        cam_top.stop();
        cam_left.stop();
        cam_right.stop();
        cv::destroyAllWindows();

        return;
    }
};


#endif //VISION_END_HPP
