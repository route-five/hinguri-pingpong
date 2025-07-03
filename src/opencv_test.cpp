#include <iostream>
#include <opencv2/opencv.hpp>
#define WINDOW_NAME "example_window"

int main() {
    std::cout << "OpenCV Version: " << cv::getVersionString() << std::endl;

    cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);
    cv::Mat img = cv::imread("OpenCV_logo.png");

    if (img.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return -1;
    }

    cv::imshow(WINDOW_NAME, img);
    cv::waitKey(0);
    cv::destroyWindow(WINDOW_NAME);

    return 0;
}