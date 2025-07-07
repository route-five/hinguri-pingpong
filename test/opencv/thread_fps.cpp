#include <opencv2/opencv.hpp>
#include <thread>

class WebcamVideoStream {
  cv::VideoCapture stream;
  std::atomic<cv::Mat *> current_frame_ptr;
  std::thread thread;
  std::atomic<bool> stopped;

public:
  explicit WebcamVideoStream(const int source = 0, const int fps = 120)
      : stream{source}, current_frame_ptr{nullptr}, stopped{false} {
    stream.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    stream.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    stream.set(cv::CAP_PROP_FPS, fps);

    const auto frame_ptr = new cv::Mat();
    stream >> *frame_ptr;

    current_frame_ptr.store(frame_ptr);
  }

  double get_prop(const int prop_id) const { return stream.get(prop_id); }

  bool is_opened() const { return stream.isOpened(); }

  void start() { thread = std::thread(&WebcamVideoStream::update, this); }

  void update() {
    while (!stopped.load()) {
      const auto new_frame = new cv::Mat();
      stream >> *new_frame;

      if (!new_frame->empty()) {
        const cv::Mat *old_frame = current_frame_ptr.exchange(new_frame);
        delete old_frame;
      } else {
        delete new_frame;
      }
    }
  }

  cv::Mat read() const {
    cv::Mat *ptr = current_frame_ptr.load();
    return ptr ? *ptr : cv::Mat();
  }

  void stop() {
    stopped.store(true);
    if (thread.joinable())
      thread.join();

    const cv::Mat *last_frame = current_frame_ptr.load();
    delete last_frame;
  }
};

int main() {
  WebcamVideoStream stream(0, 120);
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  stream.start();

  const std::string window_name = "Webcam (With Thread)";
  cv::namedWindow(window_name);

  int frame_count = 0;
  double fps = 0.0;
  auto last_time = std::chrono::high_resolution_clock::now();

  while (true) {
    cv::Mat frame = stream.read();

    if (frame.empty())
      continue;

    // FPS 계산
    frame_count++;
    auto now = std::chrono::high_resolution_clock::now();
    const auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time)
            .count();

    if (duration >= 1000) {
      fps = frame_count * 1000.0 / duration;
      frame_count = 0;
      last_time = now;
    }

    // FPS 텍스트 표시
    cv::putText(
        frame,
        std::format("FPS: {}/{}", static_cast<int>(fps),
                    static_cast<int>(stream.get_prop(cv::CAP_PROP_FPS))),
        cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0),
        2);

    // 영상 출력
    cv::imshow(window_name, frame);

    // ESC로 종료
    if (cv::waitKey(1) == 27)
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}