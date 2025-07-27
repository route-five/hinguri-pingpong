#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../src/constants.hpp"

int main() {
  // 카메라 열기
  // cv::VideoCapture cap(0);
  // if (!cap.isOpened()) {
  //   std::cerr << "❌ Failed to open camera" << std::endl;
  //   return -1;
  // }

  while (true) {
    cv::Mat frame;

    frame = cv::imread("img/test2/original/20250701_173805_150.png");
    // cap >> frame;

    if (frame.empty())
      break;

    // grayscale로 변환 (Sobel은 보통 gray에서 사용)
    cv::Mat mask, hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

    // Sobel gradient 계산
    cv::Mat grad_x, grad_y;
    cv::Sobel(mask, grad_x, CV_32F, 1, 0); // x 방향
    cv::Sobel(mask, grad_y, CV_32F, 0, 1); // y 방향

    // magnitude 계산: sqrt(grad_x^2 + grad_y^2)
    cv::Mat magnitude;
    cv::magnitude(grad_x, grad_y, magnitude);

    // magnitude를 0~255로 normalize해서 보기 좋게
    cv::Mat mag_display;
    cv::normalize(magnitude, mag_display, 0, 255, cv::NORM_MINMAX, CV_8U);

    // 두 개의 창 띄우기
    cv::imshow("Camera", frame);
    cv::imshow("Mask", mask);
    cv::imshow("Sobel Magnitude", mag_display);

    if (cv::waitKey(1) == 'q')
      break;
  }

  // cap.release();
  cv::destroyAllWindows();
  return 0;
}