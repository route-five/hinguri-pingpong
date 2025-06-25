//
// Created by 임정훈 on 25. 6. 23.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#define WINDOW_NAME "example_window"

int main() {
    cv::Mat source = cv::imread("pingpong.jpg");

    if (source.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return -1;
    }

    imshow("source", source);
    cv::Mat mask;
    // TODO: Change to fit real conditions
    cv::Scalar lowerb = cv::Scalar(33, 120, 200); //BGR
    cv::Scalar upperb = cv::Scalar(90, 180, 280); //BGR
    cv::inRange(source, lowerb, upperb, mask);

    cv::imshow(WINDOW_NAME, mask);
    cv::waitKey(0);
    cv::destroyWindow(WINDOW_NAME);
}
