#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../../src/constants.hpp"
#include "../../../src/stream/webcam_video_stream.hpp"

double get_circularity(const std::vector<cv::Point> &contour) {
  const double area = cv::contourArea(contour);
  if (area == 0)
    return 0.0;

  const double perimeter = cv::arcLength(contour, true);
  return 4 * CV_PI * area / (perimeter * perimeter);
}

int main() {
  WebcamVideoStream stream(0);
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  stream.start();

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty())
      continue;

    cv::Mat mask, hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty())
      continue;

    for (int i = 0; i < contours.size(); ++i) {
      cv::drawContours(frame, contours, i, cv::Scalar(0, 0, 255), 2);
    }

    bool is_touched = false, is_detected = false;
    double max_moment = 0.0;
    const std::vector<cv::Point> *most_contour = nullptr;

    for (const auto &contour : contours) {
      cv::Moments mu = cv::moments(contour);
      if (mu.m00 > max_moment) {
        max_moment = mu.m00;
        most_contour = &contour;
      }

      float radius;
      cv::Point2f center;
      cv::minEnclosingCircle(contour, center, radius);
      if (radius >= RADIUS_MIN) {
        cv::circle(frame, center, static_cast<int>(radius),
                   cv::Scalar(255, 0, 0), 2);
      }
    }

    if (most_contour != nullptr) {
      cv::Point2f center;
      float radius;
      cv::minEnclosingCircle(*most_contour, center, radius);

      cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 255, 0),
                 2);
    }

    // Draw legend in the bottom-left corner
    const int base_line = frame.rows - 60;
    constexpr int font = cv::FONT_HERSHEY_SIMPLEX;
    constexpr double scale = 0.6;
    constexpr int thickness = 1;

    cv::putText(frame, "Green: Most Circular One", cv::Point(10, base_line),
                font, scale, cv::Scalar(0, 255, 0), thickness, cv::LINE_AA);
    cv::putText(frame, "Blue: Enclosing Contour Circles",
                cv::Point(10, base_line + 20), font, scale,
                cv::Scalar(255, 0, 0), thickness, cv::LINE_AA);
    cv::putText(frame, "Red: Every Contours", cv::Point(10, base_line + 40),
                font, scale, cv::Scalar(0, 0, 255), thickness, cv::LINE_AA);

    cv::imshow("Webcam with FPS", frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}