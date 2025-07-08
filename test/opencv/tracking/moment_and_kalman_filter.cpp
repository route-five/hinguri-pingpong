//
// Created by Hyunseung Ryu on 25. 7. 7.
//

#include "../../../src/constants.hpp"
#include "../../../src/stream/webcam_video_stream.hpp"

#include <numbers>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

#define USE_CAM

cv::KalmanFilter
    kalman(4, 2, 0); // 4 dynamic params: x, y, dx, dy; 2 measured params: x, y
cv::Mat state(4, 1, CV_32F); // [x, y, dx, dy]
cv::Mat meas(2, 1, CV_32F);  // [x_meas, y_meas]

void init_kalman() {
  kalman.transitionMatrix =
      (cv::Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
  kalman.measurementMatrix = cv::Mat::eye(2, 4, CV_32F);
  kalman.processNoiseCov = cv::Mat::eye(4, 4, CV_32F) * 1e-2;
  kalman.measurementNoiseCov = cv::Mat::eye(2, 2, CV_32F) * 1e-1;
  kalman.errorCovPost = cv::Mat::eye(4, 4, CV_32F);

  cv::randu(kalman.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
}

// 이심률 구하기
double get_circularity(const std::vector<cv::Point> &contour) {
  const double area = cv::contourArea(contour);
  if (area == 0)
    return 0.0;

  const double perimeter = cv::arcLength(contour, true);
  return 4 * std::numbers::pi_v<double> * area / (perimeter * perimeter);
}

// 오렌지 색 공 검출 함수
void detect_orange_balls(const cv::Mat &frame) {
  // color detection frame
  cv::Mat mask, hsv;
  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
  cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

  // find contours - on binary mask (white or black)
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  for (int i = 0; i < contours.size(); ++i) {
    cv::drawContours(frame, contours, i, cv::Scalar(0, 0, 255), 2);
  }

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
      cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(255, 0, 0),
                 2);
    }
  }

  cv::Point2f center;
  if (most_contour != nullptr) {
    float radius;
    cv::minEnclosingCircle(*most_contour, center, radius);

    cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 255, 0),
               2);

    meas.at<float>(0) = center.x;
    meas.at<float>(1) = center.y;
    kalman.correct(meas);
  }

  // Always predict using Kalman Filter (even if no contour found)
  cv::Mat prediction = kalman.predict();
  cv::Point predicted_pt(prediction.at<float>(0), prediction.at<float>(1));
  cv::circle(frame, predicted_pt, 8, cv::Scalar(0, 255, 255),
             2); // Yellow prediction

  // Draw legend in the bottom-left corner
  const int base_line = frame.rows - 70;
  constexpr int font = cv::FONT_HERSHEY_SIMPLEX;
  constexpr double scale = 0.6;
  constexpr int thickness = 1;

  cv::putText(frame, "Green: Most Circular One", cv::Point(10, base_line), font,
              scale, cv::Scalar(0, 255, 0), thickness, cv::LINE_AA);
  cv::putText(frame, "Blue: Enclosing Contour Circles",
              cv::Point(10, base_line + 20), font, scale, cv::Scalar(255, 0, 0),
              thickness, cv::LINE_AA);
  cv::putText(frame, "Red: Every Contours", cv::Point(10, base_line + 40), font,
              scale, cv::Scalar(0, 0, 255), thickness, cv::LINE_AA);
  cv::putText(frame, "Yellow: Kalman Predicted Position",
              cv::Point(10, base_line + 60), font, scale,
              cv::Scalar(0, 255, 255), thickness, cv::LINE_AA);

  cv::putText(frame, std::format("m00={:.3f}", max_moment), center,
              cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2,
              cv::LINE_AA);
  cv::putText(frame, std::format("m00={:.3f}", max_moment), center,
              cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1,
              cv::LINE_AA);

  cv::imshow("Mask", mask);
  cv::imshow("Contours", frame);
}

int main() {

#ifdef USE_CAM
  WebcamVideoStream::FPS = 240;
  WebcamVideoStream stream{0};

  if (!stream.is_opened()) {
    std::cerr << "Failed to open webcam." << std::endl;
    return -1;
  }

  stream.start();
#endif

  init_kalman();

  while (true) {
#ifdef USE_CAM
    const auto frame = stream.read();
#else
    const auto frame = cv::imread("img/test2/original/20250701_173805_150.png");
#endif
    detect_orange_balls(frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

#ifdef USE_CAM
  stream.stop();
#endif

  cv::destroyAllWindows();

  return 0;
}