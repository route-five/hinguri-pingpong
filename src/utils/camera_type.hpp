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

  [[nodiscard]] std::string calibration_image_dir() const {
    return "img/chessboard/" + name_ + "/";
  }

  [[nodiscard]] std::string calibration_matrix_path() const {
    return "data/" + name_ + ".yml";
  }

  [[nodiscard]] std::string name() const {
    return name_;
  }

  bool operator==(const CameraType& other) const {
    return name_ == other.name_;
  }

private:
  std::string name_;

  explicit CameraType(const std::string& name) : name_{name} {
  }
};

const CameraType CameraType::TOP{"top"};
const CameraType CameraType::LEFT{"left"};
const CameraType CameraType::RIGHT{"right"};

#endif  // CAMERA_TYPE_HPP
