//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#include <opencv2/opencv.hpp>
#include <vector>

#include "vision/calibrator.hpp"
#include "vision/camera.hpp"
#include "vision/camera_type.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"
#include "vision/visualizer.hpp"

// TODO: 모두 통합하기

void callback(
    cv::Mat& frame,
    const std::function<void(const cv::Point2f&)>& set_pt,
    const Camera& camera,
    Calibrator& calibrator
) {
    if (frame.empty()) return;

    calibrator.undistort(frame, frame, false);

    const Tracker tracker{frame, ORANGE_MIN, ORANGE_MAX};
    const auto ret = tracker.get_camera_pos();
    if (ret.has_value()) {
        auto [center, radius] = ret.value();

        cv::circle(frame, center, radius, COLOR_GREEN, -1, cv::LINE_AA);
        set_pt(center);
    }

    const std::string fps_text = std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS));
    cv::putText(frame, fps_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA);
}

int main() {
    // (2) 3대의 카메라 타임라인 동기화
    // TODO: 카메라 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

    Camera cam_left(CameraType::LEFT, {0, 0}, {1280, 720}, 120);
    Camera cam_right(CameraType::RIGHT, {1, 0}, {1280, 720}, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        const std::string message = std::format(
            "Failed to open camera: left={}, right={}",
            cam_left.is_opened() ? "true" : "false",
            cam_right.is_opened() ? "true" : "false"
        );
        std::cerr << message << std::endl;
        return -1;
    }

    Predictor predictor;
    Calibrator calibrator_left(cam_left.get_camera_type(), cam_left.get_image_size());
    Calibrator calibrator_right(cam_right.get_camera_type(), cam_right.get_image_size());

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

    // TODO: 카메라에 공이 적어도 한 대라도 안 보일 경우 위치가 겁나 튀는 문제 해결 필요 - 이 때 kalman filter 같은 걸로 예측?

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::arrowedLine(
            frame_left, predictor.get_prev_left_point(), predictor.get_left_point(),
            COLOR_GREEN, 2, cv::LINE_AA
        );
        cv::arrowedLine(
            frame_right, predictor.get_prev_right_point(), predictor.get_right_point(),
            COLOR_GREEN, 2, cv::LINE_AA
        );

        // (4) 3D 위치 삼각측량
        const auto world_pos = predictor.get_new_world_pos();

        if (predictor.get_world_positions_size() >= 2) {
            auto world_speed = predictor.get_world_speed();

            // (5) Top 카메라와 비교해서 결과 정밀하게 비교
            // TODO: Top 카메라와 비교하는 로직 구현 필요

            // (6) Kalman filter 등의 후처리로 결과 보정
            // TODO: Kalman filter 적용 로직 구현 필요

            std::string world_pos_text = std::format(
                "World Position: ({:.2f}cm, {:.2f}cm, {:.2f}cm)",
                world_pos.x, world_pos.y, world_pos.z
            );
            std::string world_speed_text = std::format(
                "World Speed: ({:.1f}cm/s, {:.1f}cm/s, {:.1f}cm/s)",
                world_speed[0], world_speed[1], world_speed[2]
            );
            cv::putText(
                frame_left, world_pos_text, cv::Point(10, 70),
                cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA
            );
            cv::putText(
                frame_left, world_speed_text, cv::Point(10, 110),
                cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA
            );

            const auto predict_pos = predictor.get_arrive_pos();
            if (predict_pos.has_value()) {
                const auto predict_left = predictor.
                    get_camera_pos_from_world_pos(predict_pos.value(), CameraType::LEFT);
                const auto predict_right = predictor.get_camera_pos_from_world_pos(
                    predict_pos.value(), CameraType::RIGHT);

                cv::circle(frame_left, predict_left, 10, COLOR_BLUE, -1, cv::LINE_AA);
                cv::circle(frame_right, predict_right, 10, COLOR_BLUE, -1, cv::LINE_AA);
            }
        }

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        cv::imshow("Left / Right", concatenated);

        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();

    // (7) 탁구공 미래 궤적 예측
    // TODO: 미래 궤적 예측 로직 구현 필요 - 구현 거의 완료 at quadratic_regression.cpp

    // (8) 예상 도착 위치 및 각도 정보를 토대로 하드웨어에 전송할 인자 계산
    // TODO: 하드웨어 제어 인자 계산 로직 구현 필요
    /**
     * h0 = 탁구 로봇 축 자체 높이
     * r = 탁구 로봇 구동 반지름
     * x_p = 탁구공의 예상 도착 위치 x 좌표
     * z_p = 탁구공의 예상 도착 위치 z 좌표
     *
     * 탁구 로봇의 x, θ는 다음과 같음. (θ는 컴퓨터 의자 위치에서 본 좌표 평면 관점 θ, 오른쪽으로 90도 꺾인게 0)
     * x_p = x + r * cos(θ)
     * z_p = h0 + r * sin(θ)
     * => θ = asin((z_p - h0) / r)
     * => x = x_p - r * cos(θ)
     *
     * 어려운 것은, 탁구공을 얼마나 스윙을 길게 할지에 관한 (몸통 돌리는 축) 인자와 탁구채를 얼마나 눕힐지에 관한 (손목 축) 인자 계산
     */

    // (9) 하드웨어에 제어 인자 + 명령 전송
    // TODO: 하드웨어 제어 명령 전송 로직 구현 필요

    return 0;
}
