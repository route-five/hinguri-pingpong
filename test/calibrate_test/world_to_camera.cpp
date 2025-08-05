//
// Created by Hyunseung Ryu on 2025. 7. 28..
//

#include <opencv2/opencv.hpp>

#include "utils/constants.hpp"
#include "utils/draw.hpp"
#include "vision/calibrator.hpp"
#include "vision/camera.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"

void callback(
    cv::Mat& frame,
    const Camera& camera,
    const Predictor& predictor,
    const std::vector<cv::Point2f>& camera_points,
    const std::vector<cv::Point3f>& world_points
) {
    if (frame.empty()) return;

    // calibrator.undistort(frame, frame, false);

    for (auto& pt : camera_points) {
        cv::circle(frame, pt, 3, COLOR_GREEN, -1, cv::LINE_AA);
    }

    for (auto& pt : world_points) {
        cv::Point2f camera_pos = predictor.pos_3d_to_2d(camera, pt);
        cv::circle(frame, camera_pos, 3, COLOR_BLUE, -1, cv::LINE_AA);
    }
}

int main() {
    Camera cam_left(CameraType::LEFT, { 1, cv::CAP_MSMF }, 120);
    Camera cam_right(CameraType::RIGHT, { 2, cv::CAP_MSMF }, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        std::string message = std::format(
            "Failed to open camera: left={}, right={}",
            cam_left.is_opened() ? "true" : "false",
            cam_right.is_opened() ? "true" : "false"
        );
        std::cerr << message << std::endl;
        return -1;
    }

    std::vector<cv::Point2f> camera_points_left, camera_points_right;
    std::vector<cv::Point3f> world_points;

    cv::FileStorage fs_left(cam_left.get_camera_type().camera_points_path(), cv::FileStorage::READ);
    fs_left["points"] >> camera_points_left;
    fs_left.release();

    cv::FileStorage fs_right(cam_right.get_camera_type().camera_points_path(), cv::FileStorage::READ);
    fs_right["points"] >> camera_points_right;
    fs_right.release();

    cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
    fs_world["points"] >> world_points;
    fs_world.release();

    Predictor predictor;

    cam_left.set_frame_callback(
        [&predictor, &cam_left, &camera_points_left, &world_points](cv::Mat& frame) {
            callback(frame, cam_left, predictor, camera_points_left, world_points);
        });

    cam_right.set_frame_callback(
        [&predictor, &cam_right, &camera_points_right, &world_points](cv::Mat& frame) {
            callback(frame, cam_right, predictor, camera_points_right, world_points);
        });

    cam_left.start();
    cam_right.start();

    std::vector<cv::Point3f> line;
    for (int i = 0; i <= TABLE_HEIGHT; i += 10) {
        line.emplace_back(30, i, 0);
    }

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        for (auto& pt : line) {
            const auto camera_pos_left = predictor.pos_3d_to_2d(cam_left, pt);
            const auto camera_pos_right = predictor.pos_3d_to_2d(cam_right, pt);
            cv::circle(frame_left, camera_pos_left, 3, COLOR_RED, -1, cv::LINE_AA);
            cv::circle(frame_right, camera_pos_right, 3, COLOR_RED, -1, cv::LINE_AA);
        }

        Draw::put_text(concatenated, "Blue: Calculated World Points", { 10, 30 }, COLOR_BLUE);
        Draw::put_text(concatenated, "Green: Saved World Points", { 10, 60 }, COLOR_GREEN);
        Draw::put_text(concatenated, "Red: x = 30 Line Points", { 10, 90 }, COLOR_RED);

        cv::imshow("Press Q to quit", concatenated);
        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();

    return 0;
}
//
// Created by Hyunseung Ryu on 2025. 7. 28..
//

#include <opencv2/opencv.hpp>

#include "utils/constants.hpp"
#include "utils/draw.hpp"
#include "vision/calibrator.hpp"
#include "vision/camera.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"

void callback(
    cv::Mat& frame,
    const Camera& camera,
    const Predictor& predictor,
    const std::vector<cv::Point2f>& camera_points,
    const std::vector<cv::Point3f>& world_points
) {
    if (frame.empty()) return;

    // calibrator.undistort(frame, frame, false);

    for (auto& pt : camera_points) {
        cv::circle(frame, pt, 3, COLOR_GREEN, -1, cv::LINE_AA);
    }

    for (auto& pt : world_points) {
        cv::Point2f camera_pos = predictor.pos_3d_to_2d(camera, pt);
        cv::circle(frame, camera_pos, 3, COLOR_BLUE, -1, cv::LINE_AA);
    }
}

int main() {
    Camera cam_left(CameraType::LEFT, { 1, cv::CAP_MSMF }, 120);
    Camera cam_right(CameraType::RIGHT, { 2, cv::CAP_MSMF }, 120);

    if (!cam_left.is_opened() || !cam_right.is_opened()) {
        std::string message = std::format(
            "Failed to open camera: left={}, right={}",
            cam_left.is_opened() ? "true" : "false",
            cam_right.is_opened() ? "true" : "false"
        );
        std::cerr << message << std::endl;
        return -1;
    }

    std::vector<cv::Point2f> camera_points_left, camera_points_right;
    std::vector<cv::Point3f> world_points;

    cv::FileStorage fs_left(cam_left.get_camera_type().camera_points_path(), cv::FileStorage::READ);
    fs_left["points"] >> camera_points_left;
    fs_left.release();

    cv::FileStorage fs_right(cam_right.get_camera_type().camera_points_path(), cv::FileStorage::READ);
    fs_right["points"] >> camera_points_right;
    fs_right.release();

    cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
    fs_world["points"] >> world_points;
    fs_world.release();

    Predictor predictor;

    cam_left.set_frame_callback(
        [&predictor, &cam_left, &camera_points_left, &world_points](cv::Mat& frame) {
            callback(frame, cam_left, predictor, camera_points_left, world_points);
        });

    cam_right.set_frame_callback(
        [&predictor, &cam_right, &camera_points_right, &world_points](cv::Mat& frame) {
            callback(frame, cam_right, predictor, camera_points_right, world_points);
        });

    cam_left.start();
    cam_right.start();

    std::vector<cv::Point3f> line;
    for (int i = 0; i <= TABLE_HEIGHT; i += 10) {
        line.emplace_back(30, i, 0);
    }

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        for (auto& pt : line) {
            const auto camera_pos_left = predictor.pos_3d_to_2d(cam_left, pt);
            const auto camera_pos_right = predictor.pos_3d_to_2d(cam_right, pt);
            cv::circle(frame_left, camera_pos_left, 3, COLOR_RED, -1, cv::LINE_AA);
            cv::circle(frame_right, camera_pos_right, 3, COLOR_RED, -1, cv::LINE_AA);
        }

        Draw::put_text(concatenated, "Blue: Calculated World Points", { 10, 30 }, COLOR_BLUE);
        Draw::put_text(concatenated, "Green: Saved World Points", { 10, 60 }, COLOR_GREEN);
        Draw::put_text(concatenated, "Red: x = 30 Line Points", { 10, 90 }, COLOR_RED);

        cv::imshow("Press Q to quit", concatenated);
        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();

    return 0;
}
