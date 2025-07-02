#include <iostream>
#include "opencv2/opencv.hpp"
 
int main(int argc, char* argv[])
{
    std::cout << "Hello OpenCV " << CV_VERSION << std::endl;
 
    std::string img_path{ "lenna.png" };
    cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);
 
    if (img.empty()) {
        std::cout << "Could not read the image\n";
        return -1;
    }
 
    cv::namedWindow("image");
    cv::imshow("image", img);
 
    cv::waitKey(0);
 
    return 0;
}