//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#include <opencv2/opencv.hpp>

#include "../../src/utils/constants.hpp"
#include "../../src/vision/calibrator.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/predictor.hpp"
#include "../../src/vision/tracker.hpp"
#include "../../src/vision/visualizer.hpp"

namespace plt = matplot;

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

int main() {
    Camera cam_left({0, 0}, {1280, 720}, 120);
    Camera cam_right({1, 0}, {1280, 720}, 120);

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
    Calibrator calibrator_left(CameraType::LEFT, cam_left.get_image_size());
    Calibrator calibrator_right(CameraType::RIGHT, cam_right.get_image_size());

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

        auto world_pos = predictor.get_world_pos();
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

    Visualizer visualizer;
    for (const auto& pos : world_positions) {
        visualizer.add_point(pos);
    }

    visualizer.show();

    return 0;
}
