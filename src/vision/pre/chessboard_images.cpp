//
// Created by Hyunseung Ryu on 2025. 7. 22..
//

#include <filesystem>

#include "../camera.hpp"
#include "../camera_type.hpp"

namespace fs = std::filesystem;

int count_files_in_directory(const fs::path& dir_path) {
  int count = 0;
  if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
    for (const auto& entry : fs::directory_iterator(dir_path)) {
      if (fs::is_regular_file(entry.status())) {
        ++count;
      }
    }
  }

  return count;
}

int main() {
  const CameraType camera_type = CameraType::TOP;

  Camera stream({1}, {1920, 1080});
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다." << std::endl;
    return 1;
  }

  stream.start();

  // Ensure calibration image directory exists
  fs::path output_dir = camera_type.calibration_image_dir();
  if (!fs::exists(output_dir)) {
      fs::create_directories(output_dir);
  }

  int save_count =
    count_files_in_directory(camera_type.calibration_image_dir());

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty()) continue;

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // cv::putText(
    //     frame,
    //     std::format("FPS: {:.1f}/{}", stream.get_fps(),
    //                 static_cast<int>(stream.get_prop(cv::CAP_PROP_FPS))),
    //     cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0),
    //     2);

    cv::imshow("Calibration Image Capture", gray);

    const int key = cv::waitKey(1);
    if (key == 'q') break;
    if (key == ' ') {
      std::ostringstream filename;
      filename << camera_type.calibration_image_dir() << std::setfill('0')
        << std::setw(2) << save_count++ << ".png";

      if (cv::imwrite(filename.str(), gray)) {
        std::cout << "이미지 저장: " << filename.str() << std::endl;
      }
      else {
        std::cerr << "이미지 저장 실패: " << filename.str() << std::endl;
      }
    }
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}
