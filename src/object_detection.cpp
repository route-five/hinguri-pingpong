//
// Created by 임정훈 on 25. 6. 23.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Constants.hpp"
#include "utils/capture_frame.hpp"

#define WINDOW_NAME "example_window"

int main() {
    // cv::Mat source = cv::imread("pingpong.jpg");
    cv::Mat source = capture_frame();

    if (source.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return -1;
    }

    imshow("source", source);
    cv::Mat mask;
    // TODO: Change to fit real conditions
    cv::inRange(source, ORANGE_MIN, ORANGE_MAX, mask);

    cv::imshow(WINDOW_NAME, mask);
    cv::waitKey(0);
    cv::destroyWindow(WINDOW_NAME);
}
