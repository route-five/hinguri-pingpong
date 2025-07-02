#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#define WINDOW_NAME "example_window"
using namespace std;

int main() {
    cout << "Hello, OpenCV! " << endl;

    cv::Mat img = cv::imread("OpenCV_logo.png");

    if (img.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return -1;
    }

    cv::imshow(WINDOW_NAME, img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}