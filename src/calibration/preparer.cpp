//
// Created by Hyunseung Ryu on 2025. 7. 22..
//

#include "../stream/webcam_video_stream.hpp"
int main() {
  WebcamVideoStream stream({}, {1280, 720});
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다." << std::endl;
    return 1;
  }

  stream.start();

  int save_count = 0;

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty()) continue;

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::imshow("Calibration Image Capture", gray);

    const char key = cv::waitKey(1);
    if (key == 'q') break;
    if (key == ' ') {
      std::ostringstream filename;
      filename << "img/chessboard/" << std::setfill('0') << std::setw(2)
               << save_count++ << ".png";
      if (cv::imwrite(filename.str(), gray)) {
        std::cout << "이미지 저장: " << filename.str() << std::endl;
      } else {
        std::cerr << "이미지 저장 실패: " << filename.str() << std::endl;
      }
    }
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}
