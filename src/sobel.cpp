#include <iostream>
#include <opencv2/opencv.hpp>

#include "constants.hpp"

int main() {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cerr << "Can't open camera\n";
    return -1;
  }

  while (true) {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty())
      break;

    // HSV 변환 → 오렌지색 마스크
    cv::Mat hsv, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

    // Contour 기반 공 검출
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    bool found = false;
    cv::Point2f ball_center;

    for (const auto &c : contours) {
      double area = cv::contourArea(c);
      double perimeter = cv::arcLength(c, true);
      if (perimeter == 0)
        continue;

      double circularity = 4 * CV_PI * area / (perimeter * perimeter);
      if (circularity > 0.6 && area > 30) {
        cv::Moments m = cv::moments(c);
        if (m.m00 > 0) {
          ball_center = cv::Point2f(m.m10 / m.m00, m.m01 / m.m00);
          found = true;
          break;
        }
      }
    }

    if (found) {
      cv::circle(frame, ball_center, 6, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
      cv::putText(frame, "Circularity-detected", cv::Point(10, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 1,
                  cv::LINE_AA);
    } else {
      // 📌 Circularity 실패 시 → Blur intensity 기반 후보 추정
      cv::Mat grad_x, grad_y, magnitude;
      cv::Sobel(mask, grad_x, CV_32F, 1, 0);
      cv::Sobel(mask, grad_y, CV_32F, 0, 1);
      cv::magnitude(grad_x, grad_y, magnitude);

      // 최대 blur intensity 위치 찾기
      double maxVal;
      cv::Point maxLoc;
      cv::minMaxLoc(magnitude, nullptr, &maxVal, nullptr, &maxLoc);

      if (maxVal > 50) { // blur가 강한 경우만 표시
        cv::circle(frame, maxLoc, 6, cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
        cv::putText(frame, "Blur-detected", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 1,
                    cv::LINE_AA);
      } else {
        cv::putText(frame, "No ball detected", cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA);
      }
    }

    cv::imshow("Ball Tracker", frame);
    if (cv::waitKey(10) == 'q')
      break;
  }

  return 0;
}