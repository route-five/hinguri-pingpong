#include "../../src/stream/webcam_video_stream.hpp"
#include <format>
#include <opencv2/opencv.hpp>

int main() {
  WebcamVideoStream stream(0);
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  stream.start();

  const std::string window_name = "Webcam (With Thread)";
  cv::namedWindow(window_name);

  while (true) {
    cv::Mat frame = stream.read();

    if (frame.empty())
      continue;

    cv::putText(
        frame,
        std::format("FPS: {:.1f}/{}", stream.get_fps(),
                    static_cast<int>(stream.get_prop(cv::CAP_PROP_FPS))),
        cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0),
        2);

    cv::imshow(window_name, frame);

    if (cv::waitKey(1) == 27)
      break; // ESC로 종료
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}