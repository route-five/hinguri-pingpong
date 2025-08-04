//
// Created by Hyunseung Ryu on 2025. 7. 10..
//

#ifndef CONTOUR_HPP
#define CONTOUR_HPP

#include "../utils/constants.hpp"
#include <numbers>
#include <opencv2/opencv.hpp>
#include <ranges>

class Contour {
private:
  std::vector<cv::Point> points;

public:
  explicit Contour(const std::vector<cv::Point>& points) : points{points} {
  }

  Contour(const std::initializer_list<cv::Point>& list) : points{list} {
  }

  ~Contour() = default;

  void draw(cv::UMat& frame, const cv::Scalar& color) const {
    for (const auto point : points) {
      cv::circle(frame, point, 1, color, 1, cv::LINE_AA);
    }
  }

  [[nodiscard]] const std::vector<cv::Point>& get_points() const {
    return points;
  }

  [[nodiscard]] size_t size() const {
    return points.size();
  }

  [[nodiscard]] bool empty() const {
    return points.empty();
  }

  [[nodiscard]] bool zero() const {
    return area() == 0;
  }

  [[nodiscard]] double area() const {
    if (empty())
      throw std::runtime_error(
        "인자로 건네 준 contour의 정점이 0개입니다. - at Contour::area");

    return cv::contourArea(points);
  }

  [[nodiscard]] double perimeter(const bool closed = true) const {
    return cv::arcLength(points, closed);
  }

  [[nodiscard]] double circularity() const {
    if (this->empty())
      throw std::runtime_error("인자로 건네 준 contour의 정점이 0개입니다. - "
        "at Contour::circularity");

    const double area = this->area();
    if (area == 0)
      return 0.0;

    const double perimeter = this->perimeter();

    return 4 * std::numbers::pi_v<double> * area / (perimeter * perimeter);
  }

  [[nodiscard]] double distance_circularity(const double threshold = std::numeric_limits<double>::max()) const {
    const double dist = std::abs(CIRCULARITY_PERFECT - this->circularity());
    if (dist > threshold)
      return std::numeric_limits<double>::max();
    else
      return dist;
  }

  [[nodiscard]] std::pair<cv::Point2f, double> min_enclosing_circle() const {
    if (this->empty())
      throw std::runtime_error("인자로 건네 준 contour의 정점이 0개입니다. - "
        "at Contour::min_enclosing_circle");

    cv::Point2f center;
    float radius;
    cv::minEnclosingCircle(points, center, radius);

    return {center, radius};
  }

  [[nodiscard]] cv::Point2f moment() const {
    if (this->empty())
      throw std::runtime_error(
        "인자로 건네 준 contour의 정점이 0개입니다. - at Contour::moment");

    const cv::Moments mu = cv::moments(points);
    if (mu.m00 == 0)
      throw std::runtime_error(
        "Contour의 면적이 0입니다. - at Contour::moment");

    return {
      static_cast<float>(mu.m10 / mu.m00),
      static_cast<float>(mu.m01 / mu.m00)
    };
  }
};

#endif // CONTOUR_HPP
