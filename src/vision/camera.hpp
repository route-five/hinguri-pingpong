#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>

#include "camera_type.hpp"
#include "../utils/log.hpp"

using Device = struct Device_ {
  int source = 0;
  int backend = 0;
};

class Camera {
  const CameraType& camera_type;
  cv::VideoCapture stream;
  Device device;
  cv::Size frame_size;

  cv::UMat buffer_[2]; // double buffer
  std::atomic<int> active_idx{0}; // 현재 버퍼 index (lock-free)

  std::thread thread;
  std::atomic<bool> stopped{false};
  std::atomic<double> capture_fps{0.0};

  std::function<void(cv::UMat&)> frame_callback;

public:
  explicit Camera(const CameraType& camera_type, const Device& device, const int fps = 120)
    : camera_type{camera_type},
      stream{device.source, device.backend},
      device{device},
      frame_size{camera_type.resolution()} {
    stream.set(cv::CAP_PROP_BUFFERSIZE, 1);
    stream.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    stream.set(cv::CAP_PROP_FRAME_WIDTH, frame_size.width);
    stream.set(cv::CAP_PROP_FRAME_HEIGHT, frame_size.height);
    stream.set(cv::CAP_PROP_FPS, fps);

    if (!stream.isOpened()) {
      Log::error(std::format("[Camera::Camera] Failed to open camera: type={}, source={}, backend={}", camera_type.to_string(),
                             device.source, device.backend));
      throw;
    }
  }

  ~Camera() {
    stop();
    stream.release();
  }

  void set_frame_callback(const std::function<void(cv::UMat&)>& callback) {
    frame_callback = callback;
  }

  void start() {
    stopped.store(false);
    thread = std::thread(&Camera::update, this);
  }

  void stop() {
    stopped.store(true);
    if (thread.joinable()) thread.join();
  }

  void update() {
    int frame_count = 0;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!stopped.load()) {
      if (!stream.grab()) continue;

      const int next_idx = 1 - active_idx.load(std::memory_order_relaxed);
      stream.retrieve(buffer_[next_idx]); // decode to next buffer
      active_idx.store(next_idx, std::memory_order_release);

      if (frame_callback) frame_callback(buffer_[next_idx]);

      frame_count++;
      auto now = std::chrono::high_resolution_clock::now();
      const double elapsed = std::chrono::duration<double>(now - last_time).count();
      if (elapsed >= 1.0) {
        capture_fps = frame_count / elapsed;
        frame_count = 0;
        last_time = now;
      }
    }
  }

  // 얕은 복사 반환 -> 0 비용
  cv::UMat read() const {
    return buffer_[active_idx.load(std::memory_order_acquire)];
  }

  double get_fps() const {
    return capture_fps.load();
  }

  CameraType get_camera_type() const {
    return camera_type;
  }

  double get_prop(const int prop_id) const {
    return stream.get(prop_id);
  }

  bool is_opened() const {
    return stream.isOpened();
  }

  [[nodiscard]] cv::Size get_frame_size() const {
    return frame_size;
  }

  friend Camera& operator>>(Camera& cam, cv::UMat& frame) {
    frame = cam.read(); // 얕은 복사
    return cam;
  }
};

#endif  // CAMERA_HPP
