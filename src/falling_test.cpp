#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "constants.hpp"
#include "stream/webcam_video_stream.hpp"

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

  const std::string window_name = "Ball Tracking";
  cv::namedWindow(window_name);

  int frame_count = 0;
  double fps = 0.0;
  auto last_time = std::chrono::high_resolution_clock::now();
  std::deque<cv::Point2f> trails;

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty())
      continue;

    // 이하 기존 색상 검출·컨투어·표시 로직 동일
    cv::Mat mask, hsv, display;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    frame.copyTo(display);

    bool is_touched = false, is_detected = false;
    auto contour = *std::ranges::max_element(
        contours,
        [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
          return get_circularity(a) < get_circularity(b);
        });

    if (cv::contourArea(contour) < CONTOUR_AREA_MIN)
      continue;
    if (get_circularity(contour) < CIRCULARITY_MIN)
      continue;

    cv::Point2f center;
    float radius;
    cv::minEnclosingCircle(contour, center, radius);
    if (radius > RADIUS_MIN) {
      is_detected = true;
      trails.push_back(center);
      if (trails.size() > TRAILS_SIZE)
        trails.pop_front();

      cv::circle(display, center, static_cast<int>(radius),
                 cv::Scalar(0, 0, 255), 2);
      for (int i = 0; i < static_cast<int>(trails.size()) - 1; ++i) {
        cv::line(display, trails[i], trails[i + 1], cv::Scalar(255, 0, 0),
                 1 + 4 * (static_cast<double>(i) / trails.size()));
      }

      if (TOUCHED_MIN < center.y && center.y < TOUCHED_MAX) {
        is_touched = true;
        std::cout << "\a";
      }
    }

    // 프레임 표시
    std::string fps_text =
        std::format("FPS: {}/{}", static_cast<int>(stream.get_fps()),
                    static_cast<int>(stream.get_prop(cv::CAP_PROP_FPS)));
    cv::putText(display, fps_text, {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.7,
                {0, 0, 255}, 2);

    cv::imshow("Low-Res Webcam with FPS", display);
    cv::imshow("Orange Detection", mask);

    if (cv::waitKey(1) == 'q')
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}