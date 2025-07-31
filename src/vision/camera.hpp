#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>

#include "camera_type.hpp"

using Device = struct Device_ {
  int source = 0;
  int backend = 0;
};

// class Camera {
//   const CameraType& camera_type;
//   cv::VideoCapture stream;
//   Device device;
//   cv::Size frame_size;
//   std::shared_ptr<cv::Mat> current_frame_ptr;
//   mutable std::mutex frame_mutex;
//   std::thread thread;
//   std::atomic<bool> stopped;
//   std::atomic<double> capture_fps{0.0};
//   std::function<void(cv::Mat&)> frame_callback;
//
// public:
//   /**
//    * @param camera_type type of camera, e.g., CameraType::LEFT or CameraType::RIGHT
//    * @param device camara source index, backend - get from <code>python -m cv2_enumerate_cameras</code>
//    * @param fps when size {640, 480}, 120. when size {1280, 720}, 90.
//    */
//   explicit Camera(const CameraType& camera_type, const Device& device, int fps = 120)
//     : camera_type{camera_type},
//       stream{device.source, device.backend},
//       device{device},
//       frame_size{camera_type.resolution()},
//       current_frame_ptr{nullptr},
//       stopped{false} {
//     stream.set(cv::CAP_PROP_BUFFERSIZE, 1);
//     stream.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
//     stream.set(cv::CAP_PROP_FRAME_WIDTH, frame_size.width);
//     stream.set(cv::CAP_PROP_FRAME_HEIGHT, frame_size.height);
//     stream.set(cv::CAP_PROP_FPS, fps);
//
//     const auto frame_ptr = std::make_shared<cv::Mat>();
//     stream >> *frame_ptr;
//     current_frame_ptr = frame_ptr;
//   }
//
//   ~Camera() {
//     stop();
//     stream.release();
//   }
//
//   void set_frame_callback(const std::function<void(cv::Mat&)>& callback) {
//     frame_callback = callback;
//   }
//
//   CameraType get_camera_type() const {
//     return camera_type;
//   }
//
//   double get_prop(const int prop_id) const {
//     return stream.get(prop_id);
//   }
//
//   double get_fps() const {
//     return capture_fps.load();
//   }
//
//   bool is_opened() const {
//     return stream.isOpened();
//   }
//
//   void start() {
//     thread = std::thread(&Camera::update, this);
//   }
//
//   // void update() {
//   //   int frame_count = 0;
//   //   auto last_time = std::chrono::high_resolution_clock::now();
//   //
//   //   while (!stopped.load()) {
//   //     auto new_frame = std::make_shared<cv::Mat>();
//   //     stream >> *new_frame;
//   //
//   //     if (!new_frame->empty()) {
//   //       if (frame_callback) frame_callback(*new_frame);
//   //       {
//   //         std::lock_guard lock(frame_mutex);
//   //         current_frame_ptr = new_frame;
//   //       }
//   //       frame_count++;
//   //     }
//   //
//   //     auto now = std::chrono::high_resolution_clock::now();
//   //     const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();
//   //     if (elapsed >= 1000) {
//   //       capture_fps = frame_count * 1000.0 / elapsed;
//   //       frame_count = 0;
//   //       last_time = now;
//   //     }
//   //   }
//   // }
//   void update() {
//     cv::Mat frame;
//     int frame_count = 0;
//     auto last_time = std::chrono::high_resolution_clock::now();
//
//     while (!stopped.load()) {
//       if (!stream.read(frame)) continue;
//
//       if (frame_callback) frame_callback(frame);
//
//       {
//         std::lock_guard lock(frame_mutex);
//         current_frame_ptr = std::make_shared<cv::Mat>(frame); // shallow copy
//       }
//       frame_count++;
//
//       auto now = std::chrono::high_resolution_clock::now();
//       const double elapsed = std::chrono::duration<double>(now - last_time).count();
//       if (elapsed >= 1.0) {
//         capture_fps = frame_count / elapsed;
//         frame_count = 0;
//         last_time = now;
//       }
//     }
//   }
//
//   cv::Mat read() const {
//     std::shared_ptr<cv::Mat> ptr;
//     {
//       std::lock_guard lock(frame_mutex);
//       ptr = current_frame_ptr;
//     }
//     return ptr ? *ptr : cv::Mat();
//   }
//
//   friend Camera& operator>>(Camera& cam, cv::Mat& frame) {
//     frame = cam.read();
//     return cam;
//   }
//
//   void stop() {
//     stopped.store(true);
//     if (thread.joinable()) thread.join();
//     {
//       std::lock_guard lock(frame_mutex);
//       current_frame_ptr = nullptr;
//     }
//   }
//
//   [[nodiscard]] const Device& get_device() const {
//     return device;
//   }
//
//   [[nodiscard]] cv::Size get_frame_size() const {
//     return frame_size;
//   }
// };

class Camera {
  const CameraType& camera_type;
  cv::VideoCapture stream;
  Device device;
  cv::Size frame_size;

  cv::Mat buffer_[2]; // double buffer
  std::atomic<int> active_idx{0}; // 현재 버퍼 index (lock-free)

  std::thread thread;
  std::atomic<bool> stopped{false};
  std::atomic<double> capture_fps{0.0};

  std::function<void(cv::Mat&)> frame_callback;

public:
  explicit Camera(const CameraType& camera_type, const Device& device, int fps = 120)
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
      throw std::runtime_error("Failed to open camera");
    }
  }

  ~Camera() {
    stop();
    stream.release();
  }

  void set_frame_callback(const std::function<void(cv::Mat&)>& callback) {
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
  cv::Mat read() const {
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

  friend Camera& operator>>(Camera& cam, cv::Mat& frame) {
    frame = cam.read(); // 얕은 복사
    return cam;
  }
};

#endif  // CAMERA_HPP
