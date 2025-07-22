#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  // 웹캠 열기
  cv::VideoCapture cap(0); // 기본 카메라
  if (!cap.isOpened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  cap.set(cv::CAP_PROP_FPS, 120);

  const std::string window_name = "Webcam (No Thread)";
  cv::namedWindow(window_name);

  int frame_count = 0;
  double fps = 0.0;
  auto last_time = std::chrono::high_resolution_clock::now();

  while (true) {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty())
      continue;

    // FPS 계산
    frame_count++;
    auto now = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time)
            .count();

    if (duration >= 1000) {
      fps = frame_count * 1000.0 / duration;
      frame_count = 0;
      last_time = now;
    }

    // FPS 텍스트 삽입
    cv::putText(frame,
                std::format("FPS: {}/{}", static_cast<int>(fps),
                            static_cast<int>(cap.get(cv::CAP_PROP_FPS))),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(255, 0, 0), 2);

    // 영상 출력
    cv::imshow(window_name, frame);

    // ESC 키로 종료
    if (cv::waitKey(1) == 27)
      break;
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}