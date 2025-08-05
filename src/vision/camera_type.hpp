//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#ifndef CAMERA_TYPE_HPP
#define CAMERA_TYPE_HPP

#include <string>

class CameraType {
public:
  static const CameraType TOP;
  static const CameraType LEFT;
  static const CameraType RIGHT;

  CameraType(const CameraType&) = default;
  CameraType& operator=(const CameraType&) = default;

  ~CameraType() = default;

  [[nodiscard]] std::string calibration_image_dir() const {
    return "img/chessboard/" + name_ + "/";
  }

  [[nodiscard]] std::string calibration_matrix_path() const {
    return "data/calibration/" + name_ + ".yml";
  }

  [[nodiscard]] std::string camera_points_path() const {
    return "data/points/" + name_ + "_camera.yml";
  }

  [[nodiscard]] std::string projection_matrix_path() const {
    return "data/projection/" + name_ + ".yml";
  }

  [[nodiscard]] std::string name() const {
    return name_;
  }

  [[nodiscard]] cv::Size resolution() const {
    return resolution_;
  }

  bool operator==(const CameraType& other) const {
    return name_ == other.name_;
  }

  std::string to_string() const {
    return name_ + " (" + std::to_string(resolution_.width) + "x" + std::to_string(resolution_.height) + ")";
  }

private:
  std::string name_;
  cv::Size resolution_;

  explicit CameraType(const std::string& name, const cv::Size& resolution) : name_{name}, resolution_{resolution} {
  }
};

const CameraType CameraType::TOP{"top", {960, 540}};
const CameraType CameraType::LEFT{"left", {640, 360}};
const CameraType CameraType::RIGHT{"right", {640, 360}};

#endif  // CAMERA_TYPE_HPP
