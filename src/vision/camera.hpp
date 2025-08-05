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

  cv::UMat camera_matrix, dist_coeffs, map1, map2, projection_matrix, R, t;
  cv::Rect roi;

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

    if (!camera_type.read_calibration_matrix(&camera_matrix, &dist_coeffs, nullptr, nullptr, nullptr, &map1, &map2, &roi)) {
      Log::error(
        std::format("[Camera::Camera] Failed to read calibration matrix for camera: {}. Calibrate first.", camera_type.to_string()));
      throw std::runtime_error("Failed to read calibration matrix");
    }

    if (!camera_type.read_projection_matrix(&projection_matrix, &R, &t)) {
      Log::error(std::format("[Camera::Camera] Failed to read projection matrix for camera: {}. Calibrate first.",
                             camera_type.to_string()));
      throw std::runtime_error("Failed to read projection matrix");
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

  const CameraType& get_camera_type() const {
    return camera_type;
  }

  double get_prop(const int prop_id) const {
    return stream.get(prop_id);
  }

  bool is_opened() const {
    return stream.isOpened();
  }

  [[nodiscard]] const cv::Size& get_frame_size() const {
    return frame_size;
  }

  [[nodiscard]] const cv::UMat& get_camera_matrix() const {
    return camera_matrix;
  }

  [[nodiscard]] const cv::UMat& get_dist_coeffs() const {
    return dist_coeffs;
  }

  [[nodiscard]] const cv::UMat& get_map1() const {
    return map1;
  }

  [[nodiscard]] const cv::UMat& get_map2() const {
    return map2;
  }

  [[nodiscard]] const cv::UMat& get_projection_matrix() const {
    return projection_matrix;
  }

  [[nodiscard]] const cv::UMat& get_R() const {
    return R;
  }

  [[nodiscard]] const cv::UMat& get_t() const {
    return t;
  }

  [[nodiscard]] const cv::Rect& get_roi() const {
    return roi;
  }

  friend Camera& operator>>(Camera& cam, cv::UMat& frame) {
    frame = cam.read(); // 얕은 복사
    return cam;
  }
};

class StereoCamera {
private:
  const StereoCameraType& stereo_camera_type;
  const Camera& left_camera;
  const Camera& right_camera;

  cv::UMat R, T, E, F, map1x, map1y, map2x, map2y, R1, R2, P1, P2, Q;

public:
  StereoCamera(const Camera& left_camera, const Camera& right_camera)
    : stereo_camera_type{left_camera.get_camera_type(), right_camera.get_camera_type()},
      left_camera{left_camera},
      right_camera{right_camera} {
    assert(left_camera.get_frame_size() == right_camera.get_frame_size() &&
      "[StereoCamera::StereoCamera] Left and right cameras must have the same frame size.");

    if (!stereo_camera_type.read_calibration_matrix(&R, &T, &E, &F, &map1x, &map1y, &map2x, &map2y)) {
      Log::error(std::format("[StereoCamera::StereoCamera] Failed to read stereo calibration matrix for cameras: {}. Calibrate first.",
                             stereo_camera_type.to_string()));
      throw std::runtime_error("Failed to read stereo calibration matrix");
    }

    if (!stereo_camera_type.read_projection_matrix(&R1, &R2, &P1, &P2, &Q)) {
      Log::error(std::format("[StereoCamera::StereoCamera] Failed to read stereo projection matrix for cameras: {}. Calibrate first.",
                             stereo_camera_type.to_string()));
      throw std::runtime_error("Failed to read stereo projection matrix");
    }
  }

  [[nodiscard]] const Camera& get_left_camera() const {
    return left_camera;
  }

  [[nodiscard]] const Camera& get_right_camera() const {
    return right_camera;
  }

  [[nodiscard]] const StereoCameraType& get_stereo_camera_type() const {
    return stereo_camera_type;
  }

  [[nodiscard]] const cv::Size& get_frame_size() const {
    return left_camera.get_frame_size();
  }

  [[nodiscard]] const cv::UMat& get_R() const {
    return R;
  }

  [[nodiscard]] const cv::UMat& get_T() const {
    return T;
  }

  [[nodiscard]] const cv::UMat& get_E() const {
    return E;
  }

  [[nodiscard]] const cv::UMat& get_F() const {
    return F;
  }

  [[nodiscard]] const cv::UMat& get_map1x() const {
    return map1x;
  }

  [[nodiscard]] const cv::UMat& get_map1y() const {
    return map1y;
  }

  [[nodiscard]] const cv::UMat& get_map2x() const {
    return map2x;
  }

  [[nodiscard]] const cv::UMat& get_map2y() const {
    return map2y;
  }

  [[nodiscard]] const cv::UMat& get_R1() const {
    return R1;
  }

  [[nodiscard]] const cv::UMat& get_R2() const {
    return R2;
  }

  [[nodiscard]] const cv::UMat& get_P1() const {
    return P1;
  }

  [[nodiscard]] const cv::UMat& get_P2() const {
    return P2;
  }

  [[nodiscard]] const cv::UMat& get_Q() const {
    return Q;
  }
};

#endif  // CAMERA_HPP
