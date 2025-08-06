//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#ifndef CAMERA_TYPE_HPP
#define CAMERA_TYPE_HPP

#include <string>

#include "../utils/constants.hpp"

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

  bool read_calibration_matrix(cv::Mat* camera_matrix = nullptr, cv::Mat* dist_coeffs = nullptr, cv::Mat* rvecs = nullptr,
                               cv::Mat* tvecs = nullptr,
                               cv::Size* image_size = nullptr, cv::Mat* map1 = nullptr, cv::Mat* map2 = nullptr,
                               cv::Rect* roi = nullptr) const {
    cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::READ);
    if (!fs.isOpened())
      return false;

    if (camera_matrix != nullptr) {
      fs["camera_matrix"] >> *camera_matrix;
      assert(!camera_matrix->empty() && "[CameraType::read_calibration_matrix] K 행렬이 비어 있습니다.");
    }
    if (dist_coeffs != nullptr) {
      fs["dist_coeffs"] >> *dist_coeffs;
      assert(!dist_coeffs->empty() && "[CameraType::read_calibration_matrix] dist_coeffs가 비어 있습니다.");
    }
    if (rvecs != nullptr) {
      fs["rvecs"] >> *rvecs;
      assert(!rvecs->empty() && "[CameraType::read_calibration_matrix] rvecs가 비어 있습니다.");
    }
    if (tvecs != nullptr) {
      fs["tvecs"] >> *tvecs;
      assert(!tvecs->empty() && "[CameraType::read_calibration_matrix] tvecs가 비어 있습니다.");
    }
    if (image_size != nullptr) {
      fs["image_size"] >> *image_size;
      assert(!image_size->empty() && "[CameraType::read_calibration_matrix] image_size가 비어 있습니다.");
    }
    if (map1 != nullptr) {
      fs["map1"] >> *map1;
      assert(!map1->empty() && "[CameraType::read_calibration_matrix] map1가 비어 있습니다.");
    }
    if (map2 != nullptr) {
      fs["map2"] >> *map2;
      assert(!map2->empty() && "[CameraType::read_calibration_matrix] map2가 비어 있습니다.");
    }
    if (roi != nullptr) {
      fs["roi"] >> *roi;
      assert(!roi->empty() && "[CameraType::read_calibration_matrix] roi가 비어 있습니다.");
    }
    fs.release();

    return true;
  }

  bool write_calibration_matrix(const cv::Mat* camera_matrix = nullptr, const cv::Mat* dist_coeffs = nullptr,
                                const cv::Mat* rvecs = nullptr,
                                const cv::Mat* tvecs = nullptr, const cv::Size* image_size = nullptr, const double* rms = nullptr,
                                const cv::Mat* map1 = nullptr, const cv::Mat* map2 = nullptr, const cv::Rect* roi = nullptr) const {
    cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::WRITE);
    if (!fs.isOpened())
      return false;

    if (camera_matrix != nullptr) {
      assert(!camera_matrix->empty() && "[CameraType::write_calibration_matrix] K 행렬이 비어 있습니다.");
      fs << "camera_matrix" << *camera_matrix;
    }
    if (dist_coeffs != nullptr) {
      assert(!dist_coeffs->empty() && "[CameraType::write_calibration_matrix] dist_coeffs가 비어 있습니다.");
      fs << "dist_coeffs" << *dist_coeffs;
    }
    if (rvecs != nullptr) {
      assert(!rvecs->empty() && "[CameraType::write_calibration_matrix] rvecs가 비어 있습니다.");
      fs << "rvecs" << *rvecs;
    }
    if (tvecs != nullptr) {
      assert(!tvecs->empty() && "[CameraType::write_calibration_matrix] tvecs가 비어 있습니다.");
      fs << "tvecs" << *tvecs;
    }
    if (image_size != nullptr) {
      assert(!image_size->empty() && "[CameraType::write_calibration_matrix] image_size가 비어 있습니다.");
      fs << "image_size" << *image_size;
    }
    if (rms != nullptr) {
      assert(*rms >= 0 && "[CameraType::write_calibration_matrix] RMS 값이 음수입니다.");
      fs << "rms" << *rms;
    }
    if (map1 != nullptr) {
      assert(!map1->empty() && "[CameraType::write_calibration_matrix] map1가 비어 있습니다.");
      fs << "map1" << *map1;
    }
    if (map2 != nullptr) {
      assert(!map2->empty() && "[CameraType::write_calibration_matrix] map2가 비어 있습니다.");
      fs << "map2" << *map2;
    }
    if (roi != nullptr) {
      assert(!roi->empty() && "[CameraType::write_calibration_matrix] roi가 비어 있습니다.");
      fs << "roi" << *roi;
    }
    fs.release();

    return true;
  }

  // bool write_calibration_matrix(const cv::Mat* camera_matrix = nullptr, const cv::Mat* dist_coeffs = nullptr,
  //                               const cv::Mat* rvecs = nullptr,
  //                               const cv::Mat* tvecs = nullptr, const cv::Size* image_size = nullptr, const double* rms = nullptr,
  //                               const cv::Mat* map1 = nullptr, const cv::Mat* map2 = nullptr, const cv::Rect* roi = nullptr) const {
  //   cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::WRITE);
  //   if (!fs.isOpened())
  //     return false;
  //
  //   if (camera_matrix != nullptr) {
  //     assert(!camera_matrix->empty() && "[CameraType::write_calibration_matrix(U)] K 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*camera_matrix, tmp, cv::noArray());
  //     fs << "camera_matrix" << tmp;
  //   }
  //   if (dist_coeffs != nullptr) {
  //     assert(!dist_coeffs->empty() && "[CameraType::write_calibration_matrix(U)] dist_coeffs가 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*dist_coeffs, tmp, cv::noArray());
  //     fs << "dist_coeffs" << tmp;
  //   }
  //   if (rvecs != nullptr) {
  //     assert(!rvecs->empty() && "[CameraType::write_calibration_matrix(U)] rvecs가 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*rvecs, tmp, cv::noArray());
  //     fs << "rvecs" << tmp;
  //   }
  //   if (tvecs != nullptr) {
  //     assert(!tvecs->empty() && "[CameraType::write_calibration_matrix(U)] tvecs가 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*tvecs, tmp, cv::noArray());
  //     fs << "tvecs" << tmp;
  //   }
  //   if (image_size != nullptr) {
  //     assert(!image_size->empty() && "[CameraType::write_calibration_matrix(U)] image_size가 비어 있습니다.");
  //     fs << "image_size" << *image_size;
  //   }
  //   if (rms != nullptr) {
  //     assert(*rms >= 0 && "[CameraType::write_calibration_matrix(U)] RMS 값이 음수입니다.");
  //     fs << "rms" << *rms;
  //   }
  //   if (map1 != nullptr) {
  //     assert(!map1->empty() && "[CameraType::write_calibration_matrix(U)] map1가 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map1, tmp, cv::noArray());
  //     fs << "map1" << tmp;
  //   }
  //   if (map2 != nullptr) {
  //     assert(!map2->empty() && "[CameraType::write_calibration_matrix(U)] map2가 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map2, tmp, cv::noArray());
  //     fs << "map2" << tmp;
  //   }
  //   if (roi != nullptr) {
  //     assert(!roi->empty() && "[CameraType::write_calibration_matrix(U)] roi가 비어 있습니다.");
  //     fs << "roi" << *roi;
  //   }
  //   fs.release();
  //
  //   return true;
  // }

  bool read_camera_points(std::vector<cv::Point2f>& points) const {
    cv::FileStorage fs(camera_points_path(), cv::FileStorage::READ);
    if (!fs.isOpened())
      return false;

    fs["points"] >> points;
    assert(!points.empty() && "[CameraType::read_camera_points] points가 비어 있습니다.");
    fs.release();

    return true;
  }

  static bool read_world_points(std::vector<cv::Point3f>& points) {
    cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
    if (!fs_world.isOpened())
      return false;

    fs_world["points"] >> points;
    assert(!points.empty() && "[CameraType::read_world_points] points가 비어 있습니다.");

    fs_world.release();
    return true;
  }

  bool write_camera_points(const std::vector<cv::Point2f>& points) const {
    cv::FileStorage fs(camera_points_path(), cv::FileStorage::WRITE);
    if (!fs.isOpened())
      return false;

    assert(!points.empty() && "[CameraType::write_camera_points] points가 비어 있습니다.");
    fs << "points" << points;
    fs.release();

    return true;
  }

  bool read_projection_matrix(cv::Mat* P = nullptr, cv::Mat* R = nullptr, cv::Mat* t = nullptr) const {
    cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::READ);
    if (!fs.isOpened())
      return false;

    if (P != nullptr) {
      fs["projection_matrix"] >> *P;
      assert(!P->empty() && "[CameraType::read_projection_matrix] P 행렬이 비어 있습니다.");
    }
    if (R != nullptr) {
      fs["R"] >> *R;
      assert(!R->empty() && "[CameraType::read_projection_matrix] R 행렬이 비어 있습니다.");
    }
    if (t != nullptr) {
      fs["t"] >> *t;
      assert(!t->empty() && "[CameraType::read_projection_matrix] t 행렬이 비어 있습니다.");
    }
    fs.release();

    return true;
  }

  bool write_projection_matrix(const cv::Mat* P = nullptr, const cv::Mat* R = nullptr, const cv::Mat* t = nullptr) const {
    cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::WRITE);
    if (!fs.isOpened())
      return false;

    if (P != nullptr) {
      assert(!P->empty() && "[CameraType::write_projection_matrix] P 행렬이 비어 있습니다.");
      fs << "projection_matrix" << *P;
    }
    if (R != nullptr) {
      assert(!R->empty() && "[CameraType::write_projection_matrix] R 행렬이 비어 있습니다.");
      fs << "R" << *R;
    }
    if (t != nullptr) {
      assert(!t->empty() && "[CameraType::write_projection_matrix] t 행렬이 비어 있습니다.");
      fs << "t" << *t;
    }
    fs.release();

    return true;
  }

  // bool write_projection_matrix(const cv::Mat* P = nullptr, const cv::Mat* R = nullptr, const cv::Mat* t = nullptr) const {
  //   cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::WRITE);
  //   if (!fs.isOpened())
  //     return false;
  //
  //   if (P != nullptr) {
  //     assert(!P->empty() && "[CameraType::write_projection_matrix(U)] P 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*P, tmp, cv::noArray());
  //     fs << "projection_matrix" << tmp;
  //   }
  //   if (R != nullptr) {
  //     assert(!R->empty() && "[CameraType::write_projection_matrix(U)] R 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*R, tmp, cv::noArray());
  //     fs << "R" << tmp;
  //   }
  //   if (t != nullptr) {
  //     assert(!t->empty() && "[CameraType::write_projection_matrix(U)] t 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*t, tmp, cv::noArray());
  //     fs << "t" << tmp;
  //   }
  //
  //   fs.release();
  //
  //   return true;
  // }

  [[nodiscard]] std::string name() const {
    return name_;
  }

  [[nodiscard]] cv::Size resolution() const {
    return resolution_;
  }

  bool operator==(const CameraType& other) const {
    return name_ == other.name_;
  }

  [[nodiscard]] std::string to_string() const {
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

class StereoCameraType {
  const CameraType& camera_type1;
  const CameraType& camera_type2;

public:
  StereoCameraType(const CameraType& type1, const CameraType& type2)
    : camera_type1{type1}, camera_type2{type2} {
  }

  StereoCameraType(const StereoCameraType&) = default;
  StereoCameraType& operator=(const StereoCameraType&) = delete;

  ~StereoCameraType() = default;

  [[nodiscard]] std::string name() const {
    return camera_type1.name() + "_" + camera_type2.name();
  }

  [[nodiscard]] std::string calibration_matrix_path() const {
    return "data/stereo_calibration/" + name() + ".yml";
  }

  [[nodiscard]] std::string projection_matrix_path() const {
    return "data/stereo_projection/" + name() + ".yml";
  }

  bool read_calibration_matrix(cv::Mat* R = nullptr, cv::Mat* T = nullptr, cv::Mat* E = nullptr, cv::Mat* F = nullptr,
                               cv::Mat* map1x = nullptr,
                               cv::Mat* map1y = nullptr,
                               cv::Mat* map2x = nullptr, cv::Mat* map2y = nullptr) const {
    cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::READ);
    if (!fs.isOpened())
      return false;

    if (R != nullptr) {
      fs["R"] >> *R;
      assert(!R->empty() && "[StereoCameraType::read_calibration_matrix] R 행렬이 비어 있습니다.");
    }
    if (T != nullptr) {
      fs["T"] >> *T;
      assert(!T->empty() && "[StereoCameraType::read_calibration_matrix] T 행렬이 비어 있습니다.");
    }
    if (E != nullptr) {
      fs["E"] >> *E;
      assert(!E->empty() && "[StereoCameraType::read_calibration_matrix] E 행렬이 비어 있습니다.");
    }
    if (F != nullptr) {
      fs["F"] >> *F;
      assert(!F->empty() && "[StereoCameraType::read_calibration_matrix] F 행렬이 비어 있습니다.");
    }
    if (map1x != nullptr) {
      fs["map1x"] >> *map1x;
      assert(!map1x->empty() && "[StereoCameraType::read_calibration_matrix] map1x 행렬이 비어 있습니다.");
    }
    if (map1y != nullptr) {
      fs["map1y"] >> *map1y;
      assert(!map1y->empty() && "[StereoCameraType::read_calibration_matrix] map1y 행렬이 비어 있습니다.");
    }
    if (map2x != nullptr) {
      fs["map2x"] >> *map2x;
      assert(!map2x->empty() && "[StereoCameraType::read_calibration_matrix] map2x 행렬이 비어 있습니다.");
    }
    if (map2y != nullptr) {
      fs["map2y"] >> *map2y;
      assert(!map2y->empty() && "[StereoCameraType::read_calibration_matrix] map2y 행렬이 비어 있습니다.");
    }
    fs.release();

    return true;
  }

  bool write_calibration_matrix(const cv::Mat* R = nullptr, const cv::Mat* T = nullptr, const cv::Mat* E = nullptr,
                                const cv::Mat* F = nullptr,
                                const cv::Mat* map1x = nullptr, const cv::Mat* map1y = nullptr, const cv::Mat* map2x = nullptr,
                                const cv::Mat* map2y = nullptr) const {
    cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::WRITE);
    if (!fs.isOpened())
      return false;

    if (R != nullptr) {
      assert(!R->empty() && "[StereoCameraType::write_calibration_matrix] R 행렬이 비어 있습니다.");
      fs << "R" << *R;
    }

    if (T != nullptr) {
      assert(!T->empty() && "[StereoCameraType::write_calibration_matrix] T 행렬이 비어 있습니다.");
      fs << "T" << *T;
    }

    if (E != nullptr) {
      assert(!E->empty() && "[StereoCameraType::write_calibration_matrix] E 행렬이 비어 있습니다.");
      fs << "E" << *E;
    }
    if (F != nullptr) {
      assert(!F->empty() && "[StereoCameraType::write_calibration_matrix] F 행렬이 비어 있습니다.");
      fs << "F" << *F;
    }
    if (map1x != nullptr) {
      assert(!map1x->empty() && "[StereoCameraType::write_calibration_matrix] map1x 행렬이 비어 있습니다.");
      fs << "map1x" << *map1x;
    }
    if (map1y != nullptr) {
      assert(!map1y->empty() && "[StereoCameraType::write_calibration_matrix] map1y 행렬이 비어 있습니다.");
      fs << "map1y" << *map1y;
    }
    if (map2x != nullptr) {
      assert(!map2x->empty() && "[StereoCameraType::write_calibration_matrix] map2x 행렬이 비어 있습니다.");
      fs << "map2x" << *map2x;
    }

    if (map2y != nullptr) {
      assert(!map2y->empty() && "[StereoCameraType::write_calibration_matrix] map2y 행렬이 비어 있습니다.");
      fs << "map2y" << *map2y;
    }

    fs.release();

    return true;
  }

  // bool write_calibration_matrix(const cv::Mat* R = nullptr, const cv::Mat* T = nullptr, const cv::Mat* E = nullptr,
  //                               const cv::Mat* F = nullptr,
  //                               const cv::Mat* map1x = nullptr, const cv::Mat* map1y = nullptr, const cv::Mat* map2x = nullptr,
  //                               const cv::Mat* map2y = nullptr) const {
  //   cv::FileStorage fs(calibration_matrix_path(), cv::FileStorage::WRITE);
  //   if (!fs.isOpened())
  //     return false;
  //
  //   if (R != nullptr) {
  //     assert(!R->empty() && "[StereoCameraType::write_calibration_matrix(U)] R 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*R, tmp, cv::noArray());
  //     fs << "R" << tmp;
  //   }
  //   if (T != nullptr) {
  //     assert(!T->empty() && "[StereoCameraType::write_calibration_matrix(U)] T 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*T, tmp, cv::noArray());
  //     fs << "T" << tmp;
  //   }
  //   if (E != nullptr) {
  //     assert(!E->empty() && "[StereoCameraType::write_calibration_matrix(U)] E 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*E, tmp, cv::noArray());
  //     fs << "E" << tmp;
  //   }
  //   if (F != nullptr) {
  //     assert(!F->empty() && "[StereoCameraType::write_calibration_matrix(U)] F 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*F, tmp, cv::noArray());
  //     fs << "F" << tmp;
  //   }
  //   if (map1x != nullptr) {
  //     assert(!map1x->empty() && "[StereoCameraType::write_calibration_matrix(U)] map1x 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map1x, tmp, cv::noArray());
  //     fs << "map1x" << tmp;
  //   }
  //   if (map1y != nullptr) {
  //     assert(!map1y->empty() && "[StereoCameraType::write_calibration_matrix(U)] map1y 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map1y, tmp, cv::noArray());
  //     fs << "map1y" << tmp;
  //   }
  //   if (map2x != nullptr) {
  //     assert(!map2x->empty() && "[StereoCameraType::write_calibration_matrix(U)] map2x 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map2x, tmp, cv::noArray());
  //     fs << "map2x" << tmp;
  //   }
  //   if (map2y != nullptr) {
  //     assert(!map2y->empty() && "[StereoCameraType::write_calibration_matrix(U)] map2y 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*map2y, tmp, cv::noArray());
  //     fs << "map2y" << tmp;
  //   }
  //
  //   fs.release();
  //
  //   return true;
  // }

  bool read_projection_matrix(cv::Mat* R1 = nullptr, cv::Mat* R2 = nullptr, cv::Mat* P1 = nullptr, cv::Mat* P2 = nullptr,
                              cv::Mat* Q = nullptr) const {
    cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::READ);
    if (!fs.isOpened())
      return false;

    if (R1 != nullptr) {
      fs["R1"] >> *R1;
      assert(!R1->empty() && "[StereoCameraType::read_projection_matrix] R1 행렬이 비어 있습니다.");
    }
    if (R2 != nullptr) {
      fs["R2"] >> *R2;
      assert(!R2->empty() && "[StereoCameraType::read_projection_matrix] R2 행렬이 비어 있습니다.");
    }
    if (P1 != nullptr) {
      fs["P1"] >> *P1;
      assert(!P1->empty() && "[StereoCameraType::read_projection_matrix] P1 행렬이 비어 있습니다.");
    }
    if (P2 != nullptr) {
      fs["P2"] >> *P2;
      assert(!P2->empty() && "[StereoCameraType::read_projection_matrix] P2 행렬이 비어 있습니다.");
    }
    if (Q != nullptr) {
      fs["Q"] >> *Q;
      assert(!Q->empty() && "[StereoCameraType::read_projection_matrix] Q 행렬이 비어 있습니다.");
    }
    fs.release();

    return true;
  }

  bool write_projection_matrix(const cv::Mat* R1 = nullptr, const cv::Mat* R2 = nullptr, const cv::Mat* P1 = nullptr,
                               const cv::Mat* P2 = nullptr,
                               const cv::Mat* Q = nullptr) const {
    cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::WRITE);
    if (!fs.isOpened())
      return false;

    if (R1 != nullptr) {
      assert(!R1->empty() && "[StereoCameraType::write_projection_matrix] R1 행렬이 비어 있습니다.");
      fs << "R1" << *R1;
    }

    if (R2 != nullptr) {
      assert(!R2->empty() && "[StereoCameraType::write_projection_matrix] R2 행렬이 비어 있습니다.");
      fs << "R2" << *R2;
    }

    if (P1 != nullptr) {
      assert(!P1->empty() && "[StereoCameraType::write_projection_matrix] P1 행렬이 비어 있습니다.");
      fs << "P1" << *P1;
    }

    if (P2 != nullptr) {
      assert(!P2->empty() && "[StereoCameraType::write_projection_matrix] P2 행렬이 비어 있습니다.");
      fs << "P2" << *P2;
    }

    if (Q != nullptr) {
      assert(!Q->empty() && "[StereoCameraType::write_projection_matrix] Q 행렬이 비어 있습니다.");
      fs << "Q" << *Q;
    }

    fs.release();

    return true;
  }

  // bool write_projection_matrix(const cv::Mat* R1 = nullptr, const cv::Mat* R2 = nullptr, const cv::Mat* P1 = nullptr,
  //                              const cv::Mat* P2 = nullptr,
  //                              const cv::Mat* Q = nullptr) const {
  //   cv::FileStorage fs(projection_matrix_path(), cv::FileStorage::WRITE);
  //   if (!fs.isOpened())
  //     return false;
  //
  //   if (R1 != nullptr) {
  //     assert(!R1->empty() && "[StereoCameraType::write_projection_matrix(U)] R1 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*R1, tmp, cv::noArray());
  //     fs << "R1" << tmp;
  //   }
  //   if (R2 != nullptr) {
  //     assert(!R2->empty() && "[StereoCameraType::write_projection_matrix(U)] R2 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*R2, tmp, cv::noArray());
  //     fs << "R2" << tmp;
  //   }
  //   if (P1 != nullptr) {
  //     assert(!P1->empty() && "[StereoCameraType::write_projection_matrix(U)] P1 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*P1, tmp, cv::noArray());
  //     fs << "P1" << tmp;
  //   }
  //   if (P2 != nullptr) {
  //     assert(!P2->empty() && "[StereoCameraType::write_projection_matrix(U)] P2 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*P2, tmp, cv::noArray());
  //     fs << "P2" << tmp;
  //   }
  //   if (Q != nullptr) {
  //     assert(!Q->empty() && "[StereoCameraType::write_projection_matrix(U)] Q 행렬이 비어 있습니다.");
  //     cv::Mat tmp;
  //     cv::copyTo(*Q, tmp, cv::noArray());
  //     fs << "Q" << tmp;
  //   }
  //
  //   fs.release();
  //
  //   return true;
  // }

  [[nodiscard]] std::string to_string() const {
    return camera_type1.to_string() + " & " + camera_type2.to_string();
  }
};

[[nodiscard]] inline StereoCameraType operator&(const CameraType& lhs, const CameraType& rhs) {
  return {lhs, rhs};
}

#endif  // CAMERA_TYPE_HPP
