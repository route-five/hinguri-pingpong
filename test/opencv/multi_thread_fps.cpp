#include <atomic>
#include <chrono>
#include <condition_variable>
#include <format>
#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>

class FramePipeline {
  cv::VideoCapture cap;
  std::atomic<bool> running{false};

  cv::Mat latest_frame;
  std::mutex cap_mutex;

  cv::Mat processed_frame;
  std::mutex proc_mutex;
  std::condition_variable proc_cv;

  std::thread capture_thread;
  std::thread process_thread;

  int fps_target;
  std::atomic<double> capture_fps{0.0}; // ✅ 실제 fps 저장

public:
  FramePipeline(int device = 0, int fps = 120) : fps_target(fps) {
    cap.open(device);
    if (!cap.isOpened())
      throw std::runtime_error("카메라 열기 실패");
    cap.set(cv::CAP_PROP_FPS, fps_target);
  }

  void start() {
    running = true;

    // ✅ 캡처 스레드에서 FPS 계산
    capture_thread = std::thread([&]() {
      int frame_count = 0;
      auto last_time = std::chrono::high_resolution_clock::now();

      while (running) {
        cv::Mat frame;
        cap >> frame;

        if (!frame.empty()) {
          {
            std::lock_guard<std::mutex> lock(cap_mutex);
            latest_frame = frame.clone();
          }
          frame_count++;
        }

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - last_time)
                           .count();
        if (elapsed >= 1000) {
          capture_fps = frame_count * 1000.0 / elapsed;
          frame_count = 0;
          last_time = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });

    process_thread = std::thread([&]() {
      while (running) {
        cv::Mat frame_copy;
        {
          std::lock_guard<std::mutex> lock(cap_mutex);
          if (latest_frame.empty())
            continue;
          frame_copy = latest_frame.clone();
        }

        cv::putText(frame_copy,
                    std::format("FPS: {:.1f}/{}", capture_fps.load(),
                                static_cast<int>(cap.get(cv::CAP_PROP_FPS))),
                    {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 1.0, {0, 255, 0}, 2);

        {
          std::lock_guard<std::mutex> lock(proc_mutex);
          processed_frame = frame_copy;
        }
        proc_cv.notify_one();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });
  }

  bool get_processed_frame(cv::Mat &out_frame) {
    std::unique_lock<std::mutex> lock(proc_mutex);
    proc_cv.wait(lock, [&] { return !processed_frame.empty() || !running; });

    if (!running)
      return false;
    out_frame = processed_frame.clone();
    return true;
  }

  void stop() {
    running = false;
    proc_cv.notify_all();
    if (capture_thread.joinable())
      capture_thread.join();
    if (process_thread.joinable())
      process_thread.join();
    cap.release();
  }

  bool is_opened() const { return cap.isOpened(); }
};

int main() {
  try {
    FramePipeline pipeline;
    pipeline.start();

    const std::string window_name = "Multithreaded Webcam";
    cv::namedWindow(window_name);

    while (true) {
      cv::Mat frame;
      if (pipeline.get_processed_frame(frame)) {
        cv::imshow(window_name, frame);
      }

      if (cv::waitKey(1) == 27)
        break; // ESC
    }

    pipeline.stop();
    cv::destroyAllWindows();
  } catch (const std::exception &e) {
    std::cerr << "오류: " << e.what() << '\n';
    return -1;
  }

  return 0;
}