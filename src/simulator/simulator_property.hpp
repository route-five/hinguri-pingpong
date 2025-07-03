//
// Created by Hyunseung Ryu on 2025. 7. 3..
//

#ifndef SIMULATOR_PROPERTY_HPP
#define SIMULATOR_PROPERTY_HPP

#include <opencv2/opencv.hpp>

class SimulatorProperty {
 public:
  float scale = 3.0f;
  int margin = 50;

  SimulatorProperty() = default;
  explicit SimulatorProperty(const float scale) : scale{scale} {}
  explicit SimulatorProperty(const int border) : margin{border} {}
  SimulatorProperty(const float scale, const int border)
      : scale{scale}, margin{border} {}

  [[nodiscard]] cv::Point2i to_pixel(const cv::Vec2f &pt,
                                     const float max_y) const {
    return {static_cast<int>(pt[0] * scale) + margin,
            static_cast<int>((max_y - pt[1]) * scale) + margin};
  }
};

#endif  // SIMULATOR_PROPERTY_HPP
