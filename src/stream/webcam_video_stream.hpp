#ifndef WEBCAM_VIDEO_STREAM_HPP
#define WEBCAM_VIDEO_STREAM_HPP

#include <atomic>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

typedef struct {
  int source = 0;
  int backend = 0;
} Device;

class WebcamVideoStream {
  cv::VideoCapture stream;
  std::atomic<cv::Mat *> current_frame_ptr;
  std::thread thread;
  std::atomic<bool> stopped;
  std::atomic<double> capture_fps{0.0};

public:
  /**
   * @param device camara source index, backend - get from `python -m
   * cv2_enumerate_cameras`
   * @param size can be {640, 480}, **{1280, 720}**, {1920, 1080} on Logitech
   * BRIO
   * @param fps when size {640, 480}, 120. when size {1280, 720}, 90.
   */
  explicit WebcamVideoStream(const Device device, const cv::Size &size,
                             const int fps = 120)
      : stream{device.source, device.backend}, current_frame_ptr{nullptr},
        stopped{false} {
    stream.set(cv::CAP_PROP_FRAME_WIDTH, size.width);
    stream.set(cv::CAP_PROP_FRAME_HEIGHT, size.height);
    stream.set(cv::CAP_PROP_FPS, fps);

    const auto frame_ptr = new cv::Mat();
    stream >> *frame_ptr;

    current_frame_ptr.store(frame_ptr);
  }

  explicit WebcamVideoStream(const Device device, const int fps = 120)
      : stream{device.source, device.backend}, current_frame_ptr{nullptr},
        stopped{false} {
    stream.set(cv::CAP_PROP_FPS, fps);

    const auto frame_ptr = new cv::Mat();
    stream >> *frame_ptr;

    current_frame_ptr.store(frame_ptr);
  }

  ~WebcamVideoStream() {
    stream.release();
    stop();
  }

  double get_prop(const int prop_id) const { return stream.get(prop_id); }

  double get_fps() const { return capture_fps.load(); }

  bool is_opened() const { return stream.isOpened(); }

  void start() { thread = std::thread(&WebcamVideoStream::update, this); }

  void update() {
    int frame_count = 0;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!stopped.load()) {
      const auto new_frame = new cv::Mat();
      stream >> *new_frame;

      if (!new_frame->empty()) {
        const cv::Mat *old_frame = current_frame_ptr.exchange(new_frame);
        delete old_frame;
        frame_count++;
      } else {
        delete new_frame;
      }

      auto now = std::chrono::high_resolution_clock::now();
      const auto elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time)
              .count();
      if (elapsed >= 1000) {
        capture_fps = frame_count * 1000.0 / elapsed;
        frame_count = 0;
        last_time = now;
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

#endif // WEBCAM_VIDEO_STREAM_HPP