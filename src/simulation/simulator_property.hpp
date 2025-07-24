//
// Created by Hyunseung Ryu on 2025. 7. 3..
//

#ifndef SIMULATOR_PROPERTY_HPP
#define SIMULATOR_PROPERTY_HPP

#include <opencv2/opencv.hpp>

namespace Simulator {
class Property {
 public:
  float scale = 3.0f;
  int margin = 50;

  Property() = default;
  explicit Property(const float scale) : scale{scale} {}
  explicit Property(const int border) : margin{border} {}
  Property(const float scale, const int border)
      : scale{scale}, margin{border} {}

  [[nodiscard]] cv::Point2i to_pixel(const cv::Vec2f &pt,
                                     const float max_y) const {
    return {static_cast<int>(pt[0] * scale) + margin,
            static_cast<int>((max_y - pt[1]) * scale) + margin};
  }
};
}  // namespace Simulator

#endif  // SIMULATOR_PROPERTY_HPP
