#include "../../../src/constants.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cerr << "Can't open camera\n";
    return -1;
  }

  // Kalman Filter 초기화
  cv::KalmanFilter kf(4, 2, 0); // [x, y, vx, vy] 상태 / [x, y] 관측
  kf.transitionMatrix =
      (cv::Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);

  cv::setIdentity(kf.measurementMatrix);
  cv::setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-4));
  cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
  cv::setIdentity(kf.errorCovPost, cv::Scalar::all(1));

  cv::Mat state(4, 1, CV_32F);       // [x, y, vx, vy]
  cv::Mat measurement(2, 1, CV_32F); // [x, y]
  measurement.setTo(cv::Scalar(0));

  bool initialized = false;

  while (true) {
    cv::Mat frame;
    cap >> frame;

    // HSV 변환 및 오렌지색 마스크
    cv::Mat hsv, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

    // Contour 추출
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    cv::Point2f best_center;
    bool found = false;

    for (const auto &c : contours) {
      double area = cv::contourArea(c);
      double perimeter = cv::arcLength(c, true);
      if (perimeter == 0)
        continue;

      double circularity = 4 * CV_PI * area / (perimeter * perimeter);
      if (std::abs(CIRCULARITY_PERFECT - circularity) >= 0.6) {
        cv::Moments m = cv::moments(c);
        if (m.m00 == 0)
          continue;

        best_center = cv::Point2f(static_cast<float>(m.m10 / m.m00),
                                  static_cast<float>(m.m01 / m.m00));
        found = true;
        break;
      }
    }

    // Kalman 예측
    cv::Mat prediction = kf.predict();
    cv::Point predict_pt(prediction.at<float>(0), prediction.at<float>(1));

    // Kalman 보정
    if (found) {
      measurement.at<float>(0) = best_center.x;
      measurement.at<float>(1) = best_center.y;

      if (!initialized) {
        state.at<float>(0) = best_center.x;
        state.at<float>(1) = best_center.y;
        state.at<float>(2) = 0;
        state.at<float>(3) = 0;
        kf.statePost = state.clone();
        initialized = true;
      }

      kf.correct(measurement);
      cv::circle(frame, best_center, 6, cv::Scalar(0, 255, 0), 2,
                 cv::LINE_AA); // 관측점 (초록)
    }

    // 예측점 시각화
    cv::circle(frame, predict_pt, 6, cv::Scalar(0, 0, 255), 2,
               cv::LINE_AA); // 예측점 (빨강)

    // 속도 벡터 시각화
    float vx = kf.statePost.at<float>(2);
    float vy = kf.statePost.at<float>(3);
    float scale = 10.0f;

    cv::Point tip(predict_pt.x + static_cast<int>(vx * scale),
                  predict_pt.y + static_cast<int>(vy * scale));

    cv::arrowedLine(frame, predict_pt, tip, cv::Scalar(0, 255, 255), 2,
                    cv::LINE_AA); // 속도 화살표 (노랑)

    cv::putText(frame, "Red: Predicted, Green: Observed, Yellow: Velocity",
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

    cv::imshow("Kalman Tracking with Velocity", frame);
    if (cv::waitKey(10) == 'q')
      break;
  }

  return 0;
}