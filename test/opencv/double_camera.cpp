//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#include <opencv2/opencv.hpp>

#include "../../src/vision/camera.hpp"

int main() {
    Camera cam_left({0, 0}, {1080, 720}, 120);
    Camera cam_right({1, 0}, {1080, 720}, 120);

    if (!cam_left.is_opened()) {
        std::cerr << "Failed to open left camera." << std::endl;
        return -1;
    }
    if (!cam_right.is_opened()) {
        std::cerr << "Failed to open right camera." << std::endl;
        return -1;
    }

    cam_left.start();
    cam_right.start();

    while (true) {
        cv::Mat frame_left = cam_left.read(), frame_right = cam_right.read();
        if (frame_left.empty() || frame_right.empty())
            continue;

        cv::Mat concatenated;
        cv::hconcat(frame_left, frame_right, concatenated);
        cv::imshow("Left / Right", concatenated);

        if (cv::waitKey(1) == 'q') break;
    }

    cam_left.stop();
    cam_right.stop();

    return 0;
}
