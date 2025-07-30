//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#ifndef CAMERA_TYPE_HPP
#define CAMERA_TYPE_HPP

#include <string>
#include <opencv2/core/types.hpp>

class CameraType {
public:
  static const CameraType TOP;
  static const CameraType LEFT;
  static const CameraType RIGHT;

  CameraType(const CameraType&) = default;

  ~CameraType() = default;

  [[nodiscard]] const std::string& calibration_image_dir() const noexcept {
    return calibration_image_dir_;
  }

  [[nodiscard]] const std::string& calibration_matrix_path() const noexcept {
    return calibration_matrix_path_;
  }

  [[nodiscard]] const std::string& camera_points_path() const noexcept {
    return camera_points_path_;
  }

  [[nodiscard]] const std::string& projection_matrix_path() const noexcept {
    return projection_matrix_path_;
  }

  [[nodiscard]] const std::string& name() const noexcept {
    return name_;
  }

  [[nodiscard]] const cv::Size& resolution() const noexcept {
    return resolution_;
  }

  bool operator==(const CameraType& other) const noexcept {
    return name_ == other.name_;
  }

private:
  const std::string name_;
  const cv::Size resolution_;
  const std::string calibration_image_dir_;
  const std::string calibration_matrix_path_;
  const std::string camera_points_path_;
  const std::string projection_matrix_path_;

  explicit CameraType(const std::string& name, const cv::Size& resolution)
    : name_{name},
      resolution_{resolution},
      calibration_image_dir_{"img/chessboard/" + name + "/"},
      calibration_matrix_path_{"data/calibration/" + name + ".yml"},
      camera_points_path_{"data/points/" + name + "_camera.yml"},
      projection_matrix_path_{"data/projection/" + name + ".yml"} {
  }
};

const CameraType CameraType::TOP{"top", {1920, 1080}};
const CameraType CameraType::LEFT{"left", {1280, 720}};
const CameraType CameraType::RIGHT{"right", {1280, 720}};

#endif  // CAMERA_TYPE_HPP
