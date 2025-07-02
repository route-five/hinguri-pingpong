#ifndef CAPTURE_FRAME_HPP
#define CAPTURE_FRAME_HPP

#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include "../constants.hpp"

cv::Mat capture_frame() {
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera" << std::endl;
        return {};
    }

    cv::Mat img;
    cap >> img;

    if (img.empty()) {
        std::cerr << "Failed to read camera" << std::endl;
        return {};
    }

    return img;
}

#endif // CAPTURE_FRAME_HPP