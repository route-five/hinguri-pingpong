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

class Camera {
  const CameraType camera_type;
  cv::VideoCapture stream;
  Device device;
  cv::Size image_size;
  std::shared_ptr<cv::Mat> current_frame_ptr;
  mutable std::mutex frame_mutex;
  std::thread thread;
  std::atomic<bool> stopped;
  std::atomic<double> capture_fps{0.0};
  std::function<void(cv::Mat&)> frame_callback;

public:
  /**
   * @param camera_type type of camera, e.g., CameraType::LEFT or CameraType::RIGHT
   * @param device camara source index, backend - get from <code>python -m cv2_enumerate_cameras</code>
   * @param size can be {640, 480}, <b>{1280, 720}</b>, {1920, 1080} on Logitech BRIO
   * @param fps when size {640, 480}, 120. when size {1280, 720}, 90.
   */
  explicit Camera(const CameraType& camera_type, const Device device, const cv::Size& size,
                  const int fps = 120)
    : camera_type{camera_type},
      stream{device.source, device.backend},
      device{device},
      image_size{size},
      current_frame_ptr{nullptr},
      stopped{false} {
    stream.set(cv::CAP_PROP_FRAME_WIDTH, size.width);
    stream.set(cv::CAP_PROP_FRAME_HEIGHT, size.height);
    stream.set(cv::CAP_PROP_FPS, fps);

    const auto frame_ptr = std::make_shared<cv::Mat>();
    stream >> *frame_ptr;
    current_frame_ptr = frame_ptr;
  }

  explicit Camera(const CameraType& camera_type, const Device device = {}, const int fps = 120)
    : camera_type{camera_type},
      stream{device.source, device.backend},
      device{device},
      current_frame_ptr{nullptr},
      stopped{false} {
    stream.set(cv::CAP_PROP_FPS, fps);
    image_size = {
      static_cast<int>(stream.get(cv::CAP_PROP_FRAME_WIDTH)),
      static_cast<int>(stream.get(cv::CAP_PROP_FRAME_HEIGHT))
    };

    const auto frame_ptr = std::make_shared<cv::Mat>();
    stream >> *frame_ptr;
    current_frame_ptr = frame_ptr;
  }

  ~Camera() {
    stream.release();
    stop();
  }

  void set_frame_callback(const std::function<void(cv::Mat&)>& callback) {
    frame_callback = callback;
  }

  CameraType get_camera_type() const {
    return camera_type;
  }

  double get_prop(const int prop_id) const {
    return stream.get(prop_id);
  }

  double get_fps() const {
    return capture_fps.load();
  }

  bool is_opened() const {
    return stream.isOpened();
  }

  void start() {
    thread = std::thread(&Camera::update, this);
  }

  void update() {
    int frame_count = 0;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!stopped.load()) {
      auto new_frame = std::make_shared<cv::Mat>();
      stream >> *new_frame;

      if (!new_frame->empty()) {
        if (frame_callback) frame_callback(*new_frame);
        {
          std::lock_guard<std::mutex> lock(frame_mutex);
          current_frame_ptr = new_frame;
        }
        frame_count++;
      }

      auto now = std::chrono::high_resolution_clock::now();
      const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();
      if (elapsed >= 1000) {
        capture_fps = frame_count * 1000.0 / elapsed;
        frame_count = 0;
        last_time = now;
      }
    }
  }

  cv::Mat read() const {
    std::shared_ptr<cv::Mat> ptr;
    {
      std::lock_guard<std::mutex> lock(frame_mutex);
      ptr = current_frame_ptr;
    }
    return ptr ? *ptr : cv::Mat();
  }

  void stop() {
    stopped.store(true);
    if (thread.joinable()) thread.join();
    {
      std::lock_guard<std::mutex> lock(frame_mutex);
      current_frame_ptr = nullptr;
    }
  }

  [[nodiscard]] Device get_device() const {
    return device;
  }

  [[nodiscard]] cv::Size get_image_size() const {
    return image_size;
  }
};

#endif  // CAMERA_HPP
