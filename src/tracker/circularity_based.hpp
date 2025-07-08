//
// Created by Hyunseung Ryu on 2025. 7. 8..
//

#ifndef CIRCULARITY_BASED_HPP
#define CIRCULARITY_BASED_HPP

#include <opencv2/opencv.hpp>

#include "tracker.hpp"

namespace Tracker {
class CircularityBased : public AbstractTracker {
public:
  explicit CircularityBased(const cv::Mat &frame) : AbstractTracker(frame) {}

  static double circularity(const Contour &contour) {
    if (contour.empty())
      throw std::runtime_error("인자로 건네 준 contour의 정점이 0개입니다.");

    const double area = cv::contourArea(contour);
    if (area == 0)
      return 0.0;

    const double perimeter = cv::arcLength(contour, true);
    return 4 * CV_PI * area / (perimeter * perimeter);
  }

  static double similarity_to_circle(const Contour &contour) {
    return std::abs(CIRCULARITY_PERFECT - circularity(contour));
  }

  static double similarity_to_circle(const double circularity) {
    return std::abs(CIRCULARITY_PERFECT - circularity);
  }

  static Contour most_circular_contour(const std::vector<Contour> &contours) {
    const Contour *most_contour = nullptr;
    double most_circularity = DBL_MAX;

    if (contours.empty())
      throw std::runtime_error("인자로 건네 준 contours가 비어 있습니다.");

    for (const auto &contour : contours) {
      const double circularity = CircularityBased::circularity(contour);
      if (similarity_to_circle(circularity) <
          similarity_to_circle(most_circularity)) {
        most_circularity = circularity;
        most_contour = &contour;
      }
    }

    if (most_contour == nullptr)
      throw std::runtime_error(
          "가장 원형에 가까운 contour를 찾을 수 없습니다.");

    return *most_contour;
  }

  static std::pair<cv::Point2f, float>
  min_enclosing_circle(const Contour &contour) {
    if (contour.empty())
      throw std::runtime_error("인자로 건네 준 contour의 정점이 0개입니다.");

    cv::Point2f center;
    float radius;
    cv::minEnclosingCircle(contour, center, radius);

    return {center, radius};
  }
};
}; // namespace Tracker

#endif // CIRCULARITY_BASED_HPP
