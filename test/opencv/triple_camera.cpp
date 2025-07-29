//
// Created by Hyunseung Ryu on 2025. 7. 29..
//

#include "../../src/utils/draw.hpp"
#include "../../src/vision/calibrator.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/camera_type.hpp"
#include "../../src/vision/predictor.hpp"
#include "../../src/vision/tracker.hpp"

void callback(cv::Mat& frame, Tracker& tracker) {
    if (frame.empty()) return;

    tracker.update(frame);

    auto ret = tracker.get_camera_pos();
    if (ret.has_value()) {
        auto [center, radius] = ret.value();
        cv::circle(frame, center, radius, COLOR_GREEN, -1, cv::LINE_AA);
    }
}

int main() {
    cv::VideoCapture cam_top(0, 1200);
    cam_top.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cam_top.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    cam_top.set(cv::CAP_PROP_FPS, 120);

    cv::VideoCapture cam_left(1, 1200);
    cam_left.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cam_left.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cam_left.set(cv::CAP_PROP_FPS, 120);

    cv::VideoCapture cam_right(2, 1200);
    cam_right.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cam_right.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cam_right.set(cv::CAP_PROP_FPS, 120);

    Tracker tracker_left(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_right(ORANGE_MIN, ORANGE_MAX);
    Tracker tracker_top(ORANGE_MIN, ORANGE_MAX);
    cv::Mat frame_top, frame_left, frame_right;
    while (true) {
        cam_top >> frame_top;
        cam_left >> frame_left;
        cam_right >> frame_right;

        if (frame_top.empty())
            continue;
        if (frame_left.empty())
            continue;
        if (frame_right.empty())
            continue;

        callback(frame_top, tracker_top);
        callback(frame_left, tracker_left);
        callback(frame_right, tracker_right);

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);

        cv::imshow("Top Camera", frame_top);
        cv::imshow("Left/Right Camera", concatenated);

        if (cv::waitKey(1) == 'q') break;
    }
    cv::destroyAllWindows();

    return 0;
}
