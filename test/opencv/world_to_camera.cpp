//
// Created by Hyunseung Ryu on 2025. 7. 28..
//

#include <opencv2/opencv.hpp>

#include "../../src/utils/constants.hpp"
#include "../../src/vision/calibrator.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/predictor.hpp"
#include "../../src/vision/tracker.hpp"
#include "../../src/vision/visualizer.hpp"

void callback(
    cv::Mat& frame,
    const std::function<void(const cv::Point2f&)>& set_pt,
    const Camera& camera,
    Calibrator& calibrator
) {
    if (frame.empty()) return;

    calibrator.undistort(frame, frame, false);

    Tracker tracker{frame, ORANGE_MIN, ORANGE_MAX};
    const auto ret = tracker.get_camera_pos();
    if (ret.has_value()) {
        cv::circle(frame, ret->first, ret->second, COLOR_GREEN, -1, cv::LINE_AA);
        set_pt(ret->first);
    }

    std::string fps_text = std::format("FPS: {:.1f}/{:.1f}", camera.get_fps(), camera.get_prop(cv::CAP_PROP_FPS));
    cv::putText(frame, fps_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA);
}

int main() {
    Camera cam_left(CameraType::LEFT, {0, 0}, {1280, 720}, 120);
    Camera cam_right(CameraType::RIGHT, {1, 0}, {1280, 720}, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        std::string message = std::format(
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

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        auto world_pos = predictor.get_new_world_pos();
        std::string world_pos_text = std::format(
            "World Position: ({:.2f}, {:.2f}, {:.2f})",
            world_pos.x, world_pos.y, world_pos.z
        );
        cv::putText(
            frame_left, world_pos_text, cv::Point(10, 70),
            cv::FONT_HERSHEY_SIMPLEX, 1, COLOR_BLACK, 2, cv::LINE_AA
        );

        auto world_pos_left = predictor.get_camera_pos_from_world_pos(world_pos, CameraType::LEFT);
        auto world_pos_right = predictor.get_camera_pos_from_world_pos(world_pos, CameraType::RIGHT);
        cv::circle(frame_left, world_pos_left, 5, COLOR_BLUE, -1, cv::LINE_AA);
        cv::circle(frame_right, world_pos_right, 5, COLOR_BLUE, -1, cv::LINE_AA);

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        cv::imshow("Left / Right", concatenated);

        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();

    return 0;
}
