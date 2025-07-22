#include <chrono>
#include <cmath>
#include <deque>
#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>

struct FrameWithTimestamp {
  double timestamp;
  cv::Mat frame;
};

constexpr size_t MAX_BUFFER_SIZE = 30;
constexpr double SYNC_THRESHOLD_MS = 10.0;
constexpr int DELAY_MS = 100;  // 카메라 1의 시작 지연 시간

std::deque<FrameWithTimestamp> cam0_buf, cam1_buf;
std::mutex cam0_mtx, cam1_mtx;
bool running = true;

double now_ms(const std::chrono::steady_clock::time_point start) {
  return std::chrono::duration<double, std::milli>(
             std::chrono::steady_clock::now() - start)
      .count();
}

void camera_thread(const int cam_index, std::deque<FrameWithTimestamp>& buffer,
                   std::mutex& mtx,
                   const std::chrono::steady_clock::time_point start) {
  cv::VideoCapture cap(cam_index);
  if (!cap.isOpened()) {
    std::cerr << "Camera " << cam_index << " failed to open.\n";
    return;
  }

  while (running) {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) break;

    const double ts = now_ms(start);
    std::lock_guard<std::mutex> lock(mtx);
    buffer.push_back({ts, std::move(frame)});
    if (buffer.size() > MAX_BUFFER_SIZE) buffer.pop_front();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

int main() {
  auto global_start = std::chrono::steady_clock::now();

  // 0번 카메라와 1번 카메라를 각각 스레드로 실행
  std::thread t0(camera_thread, 0, std::ref(cam0_buf), std::ref(cam0_mtx),
                 global_start);

  // 1번 카메라 스레드는 0번 카메라보다 일부러 약간 지연시켜 시작
  std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
  std::thread t1(camera_thread, 1, std::ref(cam1_buf), std::ref(cam1_mtx),
                 global_start);

  while (true) {
    std::lock_guard<std::mutex> lock(cam0_mtx);
    if (cam0_buf.empty()) continue;  // cam0 버퍼가 비었으면 스킵

    const auto [timestamp, frame] =
        cam0_buf.back();  // cam0에서 가장 최근 프레임

    double min_diff = DBL_MAX;
    const FrameWithTimestamp* best_match = nullptr;
    int match_idx = -1;
    std::lock_guard<std::mutex> lock2(cam1_mtx);

    // cam1 버퍼에서 cam0 프레임과 시간차가 가장 적은 프레임 찾기
    for (int i = 0; i < static_cast<int>(cam1_buf.size()); ++i) {
      const auto& f = cam1_buf[i];
      const double diff = std::abs(f.timestamp - timestamp);

      if (diff < min_diff && diff < SYNC_THRESHOLD_MS) {
        min_diff = diff;
        best_match = &f;
        match_idx = i;
      }
    }

    if (best_match != nullptr) {
      // 동기화된 프레임 타임스탬프 출력
      std::cout << "SYNCED: cam0 = " << timestamp
                << " ms, cam1 = " << best_match->timestamp << " ms (idx "
                << match_idx << ")\n";

      // 두 프레임을 수평으로 붙여서 화면에 표시
      cv::Mat display;
      cv::hconcat(frame, best_match->frame, display);
      cv::imshow("Synced Frames", display);

      // ESC 키 누르면 종료
      if (cv::waitKey(1) == 27) break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }

  running = false;
  t0.join();
  t1.join();
  return 0;
}