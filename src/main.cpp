//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#include <opencv2/opencv.hpp>
#include <atomic>
#include <optional>
#include <thread>

#include "vision/calibrator.hpp"
#include "vision/camera.hpp"
#include "vision/camera_type.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"
#include "vision/visualizer.hpp"

struct ControlInput {
    double x;
    double z;
    double angle;
    double velocity;
};

std::mutex control_mutex;
std::optional<ControlInput> shared_control_input;
std::atomic<bool> new_control_input{false};

void callback(
    cv::Mat& frame,
    const std::function<void(cv::Point2f&)>& set_pt,
    const Camera& camera,
    Calibrator& calibrator
) {
    if (frame.empty()) return;

    calibrator.undistort(frame, frame, false);

    Tracker tracker{frame};
    tracker.set_color_mask(ORANGE_MIN, ORANGE_MAX);

    const std::vector<Contour> contours = tracker.find_contours();
    const auto most_contour = tracker.most_circular_contour(contours);

    if (most_contour.has_value() && !most_contour->empty()) {
        auto [pt, radius] = most_contour->min_enclosing_circle();

        if (radius > RADIUS_MIN) {
            cv::circle(frame, pt, radius, COLOR_GREEN, -1, cv::LINE_AA);
            set_pt(pt);
        }
    }

    std::string fps_text = std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS));
    cv::putText(frame, fps_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA);
}

void vision_end() {
    // (2) 3대의 카메라 타임라인 동기화
    // TODO: 카메라 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

    Camera cam_left({0, 0}, {1280, 720}, 120);
    Camera cam_right({1, 0}, {1280, 720}, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        const std::string message = std::format(
            "Failed to open camera: left={}, right={}",
            cam_left.is_opened() ? "true" : "false",
            cam_right.is_opened() ? "true" : "false"
        );
        std::cerr << message << std::endl;
        return;
    }

    Predictor predictor;
    Calibrator calibrator_left(CameraType::LEFT, cam_left.get_image_size());
    Calibrator calibrator_right(CameraType::RIGHT, cam_right.get_image_size());

    // (3) 탁구공 센터 검출 (예: blob)
    // TODO: 공 검출 로직 구현 필요 - 배경 제거, blob?
    cam_left.set_frame_callback([&predictor, &cam_left, &calibrator_left](cv::Mat& frame) {
        callback(frame, [&predictor](const cv::Point2f& pt) {
            predictor.set_point_left(pt);
        }, cam_left, calibrator_left);
    });
    cam_right.set_frame_callback([&predictor, &cam_right, &calibrator_right](cv::Mat& frame) {
        callback(frame, [&predictor](const cv::Point2f& pt) {
            predictor.set_point_right(pt);
        }, cam_right, calibrator_right);
    });

    cam_left.start();
    cam_right.start();

    std::vector<cv::Point3f> world_positions;

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        // (4) 3D 위치 삼각측량
        auto world_pos = predictor.get_world_pos();

        static bool has_sent_command = false;

        if (!has_sent_command && std::abs(world_pos.y - TABLE_HEIGHT / 4.0) < 1.0) {
            {
                std::lock_guard<std::mutex> lock(control_mutex);
                ControlInput input{
                    world_pos.x,
                    world_pos.z,
                    0.0, // TODO: 추후 예측각도 계산 필요
                    0.0 // TODO: 추후 속도 계산 필요
                };

                shared_control_input = input;
                new_control_input = true;
            }

            has_sent_command = true;
        }

        // (5) Top 카메라와 비교해서 결과 정밀하게 비교
        // TODO: Top 카메라와 비교하는 로직 구현 필요

        // (6) Kalman filter 등의 후처리로 결과 보정
        // TODO: Kalman filter 적용 로직 구현 필요

        world_positions.push_back(world_pos);

        std::string world_pos_text = std::format(
            "World Position: ({:.2f}, {:.2f}, {:.2f})",
            world_pos.x, world_pos.y, world_pos.z
        );

        cv::putText(
            concatenated, world_pos_text, cv::Point(10, 70),
            cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA
        );

        cv::imshow("Left / Right", concatenated);

        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();
}

void control_end() {
    DynamixelActuator topActuator(TOP_MOTOR_ID);
    DynamixelActuator midActuator(MID_MOTOR_ID);
    DynamixelActuator botActuator(BOT_MOTOR_ID);
    CLinear_actu linearActuator;

    // Perform initialization only once
    if (!topActuator.initialize() || !midActuator.initialize() ||
        !botActuator.initialize()) {
        std::cerr << "Failed to initialize one or more motors" << std::endl;
        return;
    }

    while (true) {
        if (!new_control_input.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        ControlInput input{};
        {
            std::lock_guard<std::mutex> lock(control_mutex);
            if (!shared_control_input.has_value()) continue;
            input = shared_control_input.value();
            shared_control_input.reset();
            new_control_input = false;
        }

        double target_x = input.x;
        double target_z = input.z;
        double target_angle = input.angle;
        double target_vel = input.velocity;

        double x, t, m, b, p = 1;
        double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
        double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;

        double theta = acos((target_z - h) / r);
        x = target_x - r * sin(theta);
        if (target_x < 0) p = -p;

        if (target_x == -1000) break;

        topActuator.move_by_degrees(rad_to_deg(-target_angle * p));
        midActuator.move_by_degrees(rad_to_deg(theta * p));
        botActuator.move_by_degrees(-90 * p);
        linearActuator.move_actu(IS_REVERSED ? -x : x);
        botActuator.move_by_degrees(30 * p);

        // go home
        topActuator.move_by_degrees(0);
        midActuator.move_by_degrees(0);
        botActuator.move_by_degrees(0);
        linearActuator.move_actu(0);
    }

    // Close the shared port once
    sharedPortHandler->closePort();
}

int main() {
    std::thread vision_thread(vision_end);
    std::thread control_thread(control_end);

    vision_thread.join();
    control_thread.join();

    return 0;
}
