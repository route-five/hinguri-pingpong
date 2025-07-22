#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../../src/constants.hpp"
#include "../../../src/stream/webcam_video_stream.hpp"
#include "../../../src/tracker/tracker.hpp"

int main() {
  WebcamVideoStream stream({});
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  // clang-format off
  // TODO: kalman filter는 카메라 좌표에 쓸 게 아니라 삼각측량 이후 실제 3차원
  //  좌표계에 Predictor 클래스에 적용해야할 듯 - https://github.com/robot-pingpong/robot-pingpong-cpp/blob/main/src/predictor.cpp
  // clang-format on

  // 4 dynamic params: x, y, dx, dy; 2 measured params: x, y
  cv::KalmanFilter kf(4, 2, 0);
  cv::Mat state(4, 1, CV_32F); // [x, y, dx, dy]
  cv::Mat meas(2, 1, CV_32F);  // [x_meas, y_meas]

  // clang-format off
  kf.transitionMatrix = (
    cv::Mat_<float>(4, 4) <<
    1, 0, 1, 0,
    0, 1, 0, 1,
    0, 0, 1, 0,
    0, 0, 0, 1
  );
  // clang-format on
  kf.measurementMatrix = cv::Mat::eye(2, 4, CV_32F);
  kf.processNoiseCov = cv::Mat::eye(4, 4, CV_32F) * 1e-2;
  kf.measurementNoiseCov = cv::Mat::eye(2, 2, CV_32F) * 1e-1;
  kf.errorCovPost = cv::Mat::eye(4, 4, CV_32F);

  cv::randu(kf.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));

  stream.start();

  double radius = 20;
  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty())
      continue;

    Tracker tracker{frame};
    tracker.set_color_mask(ORANGE_MIN, ORANGE_MAX);

    const std::vector<Contour> contours = tracker.find_contours();
    const auto most_contour = tracker.most_circular_contour(contours);

    // Always predict using Kalman Filter (even if no contour found)
    cv::Mat prediction = kf.predict();
    cv::Point predicted_pt(prediction.at<float>(0), prediction.at<float>(1));
    cv::circle(frame, predicted_pt, radius, COLOR_YELLOW, -1,
               cv::LINE_AA); // Yellow prediction

    if (most_contour.has_value()) {
      auto [center, r] = most_contour->min_enclosing_circle();
      radius = r;

      cv::circle(frame, center, static_cast<int>(radius), COLOR_RED, -1,
                 cv::LINE_AA);

      meas.at<float>(0) = center.x;
      meas.at<float>(1) = center.y;
      kf.correct(meas);
    }

    // Draw corrected point (green)
    cv::Point corrected_pt(kf.statePost.at<float>(0),
                           kf.statePost.at<float>(1));
    cv::circle(frame, corrected_pt, radius, COLOR_GREEN, -1, cv::LINE_AA);

    // Draw legend in the bottom-left corner
    const int base_line = frame.rows - 10;
    constexpr int font = cv::FONT_HERSHEY_SIMPLEX;
    constexpr double scale = 1;
    constexpr int thickness = 1;

    cv::putText(frame, "Red: Measurement", cv::Point(10, base_line - 90), font,
                scale, COLOR_RED, thickness, cv::LINE_AA);
    cv::putText(frame, "Green: Corrected", cv::Point(10, base_line - 60), font,
                scale, COLOR_GREEN, thickness, cv::LINE_AA);
    cv::putText(frame, "Yellow: Prediction", cv::Point(10, base_line - 30),
                font, scale, COLOR_YELLOW, thickness, cv::LINE_AA);
    cv::putText(frame,
                std::format("FPS: {}/{}", stream.get_fps(),
                            stream.get_prop(cv::CAP_PROP_FPS)),
                cv::Point(10, base_line), font, scale, cv::Scalar(0, 0, 0),
                thickness, cv::LINE_AA);

    cv::imshow("Circularity + Kalman filter", frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}