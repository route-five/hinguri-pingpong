//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#include <opencv2/opencv.hpp>
#include <vector>

#include "utils/draw.hpp"
#include "vision/bridge.hpp"
#include "vision/camera.hpp"
#include "vision/camera_type.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"
#include "vision/visualizer.hpp"

#define DEBUG
#define IMSHOW

bool has_sent = false; // 전송 여부를 나타내는 플래그

void frame_callback(
    cv::Mat& frame,
    const Camera& camera,
    Tracker& tracker,
    Predictor& predictor,
    void (Predictor::*set_pt)(const std::optional<cv::Point2f>&)
) {
    if (frame.empty()) return;
    tracker << frame;

    const auto camera_pos = tracker.get_camera_pos();
    (predictor.*set_pt)(camera_pos.has_value() ? std::make_optional(camera_pos.value().first) : std::nullopt);

#ifdef DEBUG
    if (camera_pos.has_value()) {
        Draw::put_circle(frame, camera_pos.value().first, camera_pos.value().second, COLOR_GREEN);
    }

    Draw::put_text(
        frame,
        std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS)),
        {10, 20}
    );
#endif
}

int main() {
    // (2) 3대의 카메라 타임라인 동기화
    // TODO: 카메라 레이턴시, 타임라인 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

#pragma region Initialization
    Camera cam_top(CameraType::TOP, {1, 1200}, 120);
    Camera cam_left(CameraType::LEFT, {2, 1200}, 120);
    Camera cam_right(CameraType::RIGHT, {0, 1200}, 120);
    Tracker tracker_top(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_left(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_right(ORANGE_MIN, ORANGE_MAX);
    Predictor predictor;
#pragma endregion

#pragma region Preparation
    if (!cam_top.is_opened() || !cam_left.is_opened() || !cam_right.is_opened()) {
        const std::string message = std::format(
            "Failed to open camera: top={}, left={}, right={}",
            cam_top.is_opened() ? "opened" : "fail",
            cam_left.is_opened() ? "opened" : "fail",
            cam_right.is_opened() ? "opened" : "fail"
        );
        std::cerr << message << std::endl;
        return -1;
    }
    // (3) 탁구공 센터 검출
    cam_top.set_frame_callback([&cam_top, &tracker_top, &predictor](cv::Mat& frame) {
        frame_callback(frame, cam_top, tracker_top, predictor, &Predictor::set_point_top);
    });
    cam_left.set_frame_callback([&cam_left, &tracker_left, &predictor](cv::Mat& frame) {
        frame_callback(frame, cam_left, tracker_left, predictor, &Predictor::set_point_left);
    });
    cam_right.set_frame_callback([&cam_right, &tracker_right, &predictor](cv::Mat& frame) {
        frame_callback(frame, cam_right, tracker_right, predictor, &Predictor::set_point_right);
    });

    cam_top.start();
    cam_left.start();
    cam_right.start();
#pragma endregion

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
        // 하드웨어에 전송할 위치 계산
        else if (0 <= world_pos.x && world_pos.x <= TABLE_WIDTH &&
            0 <= world_pos.y &&
            0 <= world_pos.z &&
            !has_sent) {
            {
                auto [x, theta, swing_length, wrist_angle, use_right_hand] = Bridge::convert(predict_arrive_pos);

                std::cout << "Predicted position: " << predict_arrive_pos << std::endl;
                std::cout << "Broadcast to hardware: " << std::endl
                    << "\tx: " << x << " cm" << std::endl
                    << "\ttheta: " << theta << " deg" << std::endl
                    << "\tswing_length: " << swing_length << " deg" << std::endl
                    << "\twrist_angle: " << wrist_angle << " deg" << std::endl;

                has_sent = true;
            }
        }

        // 탁구대 밖에 있으면 전송 여부 초기화
        if (has_sent && !(0 <= world_pos.x && world_pos.x <= TABLE_WIDTH && 0 <= world_pos.y && world_pos.y <=
            TABLE_HEIGHT)) {
            has_sent = false;
        }

#ifdef DEBUG
        // 실제 도착 위치 저장
        if (std::abs(world_pos.y - TABLE_HEIGHT) < 5) {
            real_arrive_pos = world_pos;
        }
#endif

#pragma region Legend
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
            {10, 80},
            COLOR_WHITE
        );

        // 예상 도착 위치 시각화
        Draw::put_circle(frame_top, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::TOP), 10, COLOR_BLUE);
        Draw::put_circle(frame_left, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::LEFT), 10, COLOR_BLUE);
        Draw::put_circle(frame_right, predictor.pos_3d_to_2d(predict_arrive_pos, CameraType::RIGHT), 10, COLOR_BLUE);
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
        Draw::put_text_border(
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
#pragma endregion

#pragma region Imshow
#ifdef IMSHOW
        cv::Mat frame_top_resized, frame_left_resized, frame_right_resized;
        cv::resize(frame_top, frame_top_resized, {}, 0.4, 0.4);
        cv::resize(frame_left, frame_left_resized, {}, 0.5, 0.5);
        cv::resize(frame_right, frame_right_resized, {}, 0.5, 0.5);

        // Create a final frame with left/right on top and top frame centered below
        int top_width = frame_left_resized.cols + frame_right_resized.cols;
        int max_width = std::max(top_width, frame_top_resized.cols);
        int total_height = std::max(frame_left_resized.rows, frame_right_resized.rows) + frame_top_resized.rows;

        cv::Mat final_frame = cv::Mat::zeros(total_height, max_width, frame_left_resized.type());

        // Copy left on top-left
        frame_left_resized.copyTo(final_frame(cv::Rect(0, 0, frame_left_resized.cols, frame_left_resized.rows)));
        // Copy right on top-right (next to left)
        frame_right_resized.copyTo(final_frame(cv::Rect(frame_left_resized.cols, 0, frame_right_resized.cols,
                                                        frame_right_resized.rows)));
        // Copy top centered below
        int x_offset = (max_width - frame_top_resized.cols) / 2;
        frame_top_resized.copyTo(final_frame(cv::Rect(
            x_offset, std::max(frame_left_resized.rows, frame_right_resized.rows), frame_top_resized.cols,
            frame_top_resized.rows)));

        cv::imshow("Combined", final_frame);

        if (cv::waitKey(1) == 'q') break;
#endif
#pragma endregion
    }

    cam_top.stop();
    cam_left.stop();
    cam_right.stop();
    cv::destroyAllWindows();

    return 0;
}
