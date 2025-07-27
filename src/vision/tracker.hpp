//
// Created by Hyunseung Ryu on 2025. 7. 8..
//

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "../utils/constants.hpp"
#include "contour.hpp"
#include <opencv2/opencv.hpp>
#include <utility>

// TODO: 배경 제거 메소드 사용해보자

class Tracker {
protected:
  cv::Mat frame;
  cv::Mat color_mask;

public:
  // const 아님.
  explicit Tracker(cv::Mat& frame) : frame(frame) {
  }

  cv::Mat& get_frame() {
    return frame;
  }

  cv::Mat& get_color_mask() {
    return color_mask;
  }

  void set_color_mask(const cv::Scalar& lower, const cv::Scalar& upper) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    color_mask.release();
    cv::inRange(hsv, lower, upper, color_mask);
  }

  [[nodiscard]] std::vector<Contour> find_contours() const {
    std::vector<std::vector<cv::Point>> contour_list;
    cv::findContours(color_mask, contour_list, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return std::move(std::ranges::to<std::vector<Contour>>(
      contour_list |
      std::views::transform([](const auto& c) {
        return Contour{c};
      })
    ));
  }

  static std::optional<Contour> most_circular_contour(const std::vector<Contour>& contours) {
    std::optional<Contour> most_contour = std::nullopt;

    for (const auto& contour : contours) {
      if (contour.empty() || contour.zero())
        continue;

      if (!most_contour.has_value() || most_contour->distance_circularity() > contour.distance_circularity()) {
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
};

#endif // TRACKER_HPP
