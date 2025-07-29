//
// Created by Hyunseung Ryu on 2025. 7. 8..
//

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "../utils/constants.hpp"
#include "contour.hpp"
#include <opencv2/opencv.hpp>
#include <utility>

class Tracker {
protected:
  const cv::Scalar lower_color_bound;
  const cv::Scalar upper_color_bound;
  const cv::Ptr<cv::BackgroundSubtractor> back_sub = cv::createBackgroundSubtractorMOG2();
  cv::Mat frame;
  cv::Mat moving_mask;
  cv::Mat moving_color_mask;

public:
  explicit Tracker(
    const cv::Scalar& lower_color_bound,
    const cv::Scalar& upper_color_bound
  ): lower_color_bound{lower_color_bound},
     upper_color_bound{upper_color_bound} {
  }

  void update(const cv::Mat& new_frame) {
    assert(!new_frame.empty() && "Frame cannot be empty - at Tracker::constructor");

    new_frame.copyTo(frame);

    // 배경 제거 (subtraction)
    back_sub->apply(frame, moving_mask, 0); // 배경 고정
    cv::threshold(moving_mask, moving_mask, 200, 255, cv::THRESH_BINARY);

    // morphology로 노이즈 제거
    cv::erode(moving_mask, moving_mask, cv::Mat(), cv::Point(-1, -1), 1);
    cv::dilate(moving_mask, moving_mask, cv::Mat(), cv::Point(-1, -1), 2);

    // frame에서 움직이는 부분만 추출
    cv::Mat moving_part;
    cv::bitwise_and(frame, frame, moving_part, moving_mask);

    // 색상 마스크 생성
    cv::Mat hsv;
    cv::cvtColor(moving_part, hsv, cv::COLOR_BGR2HSV);

    // 움직이는 부분의 색상 마스크 생성
    moving_color_mask.release();
    cv::inRange(hsv, lower_color_bound, upper_color_bound, moving_color_mask);
  }

  cv::Mat& get_frame() {
    return frame;
  }

  cv::Mat& get_moving_mask() {
    return moving_mask;
  }

  cv::Mat& get_moving_color_mask() {
    return moving_color_mask;
  }

  [[nodiscard]] std::vector<Contour> find_contours() const {
    std::vector<std::vector<cv::Point>> contour_list;
    cv::findContours(moving_color_mask, contour_list, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return std::move(std::ranges::to<std::vector<Contour>>(
      contour_list |
      std::views::transform([](const auto& c) {
        return Contour{c};
      })
    ));
  }

  static std::optional<Contour> most_circular_contour(const std::vector<Contour>& contours,
                                                      const double threshold = CIRCULARITY_THRESHOLD) {
    std::optional<Contour> most_contour = std::nullopt;

    for (const auto& contour : contours) {
      if (contour.empty() || contour.zero())
        continue;

      const double dist = contour.distance_circularity(threshold);
      if (dist < threshold && (!most_contour.has_value() || most_contour->distance_circularity() > dist)) {
        most_contour = contour;
      }
    }

    return most_contour;
  }

  static std::optional<Contour> largest_contour(const std::vector<Contour>& contours) {
    std::optional<Contour> largest_contour = std::nullopt;

    for (const auto& contour : contours) {
      if (contour.empty() || contour.zero())
        continue;

      if (!largest_contour.has_value() || largest_contour->area() < contour.area()) {
        largest_contour = contour;
      }
    }

    return largest_contour;
  }

  /**
   * @brief 타겟의 위치를 카메라 좌표계에서 찾습니다.
   * @return 가장 원형에 가까운 컨투어의 (최소 외접원 중심 좌표, 반지름)
   */
  [[nodiscard]] std::optional<std::pair<cv::Point2f, double>> get_camera_pos() const {
    const std::vector<Contour> contours = find_contours();
    const auto most_contour = most_circular_contour(contours);

    if (most_contour.has_value()) {
      const auto ret = most_contour->min_enclosing_circle();
      if (ret.second > RADIUS_MIN) {
        return ret;
      };
    }

    return std::nullopt;
  }
};

#endif // TRACKER_HPP
