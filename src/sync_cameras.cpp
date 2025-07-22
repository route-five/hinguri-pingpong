//
// Created by Hyunseung Ryu on 2025. 7. 22..
//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

struct FrameWithTimestamp {
  double timestamp;
  cv::Mat frame;
};

void camera_reader(const int cam_index, std::vector<FrameWithTimestamp>& buffer,
                   std::mutex& mtx,
                   const std::chrono::steady_clock::time_point global_start) {
  cv::VideoCapture cap(cam_index);
  if (!cap.isOpened()) {
    std::cerr << "Camera " << cam_index << " failed to open." << std::endl;
    return;
  }

  cap.set(cv::CAP_PROP_FPS, 120);
  while (true) {
    auto now = std::chrono::steady_clock::now();
    const double ts =
        std::chrono::duration<double, std::milli>(now - global_start).count();
    cap.grab();
    cv::Mat frame;
    if (!cap.retrieve(frame)) {
      std::cout << "Camera " << cam_index
                << " failed to retrieve frame at index " << buffer.size()
                << std::endl;
      break;
    }

    std::lock_guard<std::mutex> lock(mtx);
    buffer.push_back({ts, frame.clone()});

    if (buffer.size() >= 100) break;
  }
  cap.release();
}

/**
 * 여러 카메라의 녹화 시작 시간이 다를 수 있으므로, 프레임 인덱스가 다를 수
 * 있음. 이를 해결하고자 <code>ref_time</code>을 기준으로 이와 가장 가까운 각
 * 버퍼의 프레임부터 그 이후의 프레임만 얻도록 버퍼의 앞 쪽을 자르는 함수.
 * @param buffer 프레임과 타임스탬프가 저장된 벡터
 * @param ref_time 기준 타임스탬프.
 * @return 버퍼에서 <code>ref_time</code>와 가장 가까운 프레임부터 끝까지
 */
std::vector<FrameWithTimestamp> align_frames(
    const std::vector<FrameWithTimestamp>& buffer, const double ref_time) {
  if (buffer.empty()) return {};

  const auto it =
      std::lower_bound(buffer.begin(), buffer.end(), ref_time,
                       [](const FrameWithTimestamp& f, const double t) {
                         return f.timestamp < t;
                       });
  size_t idx = std::distance(buffer.begin(), it);

  // idx-1 또는 idx 둘 중 어느 것이 ref_time에 더 가까운지 확인
  if (idx > 0) {
    if (std::abs(buffer[idx - 1].timestamp - ref_time) <
        std::abs(buffer[idx].timestamp - ref_time)) {
      --idx;
    }
  }

  std::vector<FrameWithTimestamp> aligned(buffer.begin() + idx, buffer.end());
  return aligned;
}

// clang-format off
/**
 * 각 카메라의 버퍼에서 가장 늦은 첫 프레임을 기준으로 모든 카메라의 버퍼를
 * <code>align_frames</code>에 넣어 동기화하는 함수.
 * @param buffers 각 카메라의 프레임과 타임스탬프가 저장된 벡터인 버퍼들의 벡터
 * @return 동기화된 버퍼들의 벡터
 *
 * @note 3개의 카메라가 있고, 각 카메라의 버퍼에 프레임이 다음과 같이 있다고 가정.
 * <code>
 * buffers[0] = [{101ms}, {105ms}, ...]
 * buffers[1] = [{102ms}, {108ms}, ...]
 * buffers[2] = [{110ms}, {115ms}, ...]
 * </code>
 * 이럴 경우, 가장 늦은 첫 프레임의 타임스탬프인 <code>ref_time</code>는 110ms가 된다.
 * <code>
 * synced[0] = [{closest to 110ms}, ...] → ex: {115ms}부터
 * synced[1] = [{closest to 110ms}, ...] → ex: {110ms}부터
 * synced[2] = [{110ms}, ...] (변화 없음)
 * </code>
 */
// clang-format on
std::vector<std::vector<FrameWithTimestamp>> synchronize_buffers(
    const std::vector<std::vector<FrameWithTimestamp>>& buffers) {
  assert(buffers.size() > 0 &&
         "Buffers cannot be empty - at synchronize_buffers()");

  // 가장 늦은 첫 프레임의 타임스탬프 찾기
  double ref_time = DBL_MIN;
  for (const auto& buf : buffers) {
    if (buf.empty()) continue;

    if (buf.front().timestamp > ref_time) {
      ref_time = buf.front().timestamp;
    }
  }

  // ref_time 기준으로 각 버퍼의 프레임을 정렬
  std::vector<std::vector<FrameWithTimestamp>> result;
  for (const auto& buf : buffers) {
    result.push_back(align_frames(buf, ref_time));
  }

  return result;
}

int main() {
  constexpr int cam_count = 1;
  std::vector<std::vector<FrameWithTimestamp>> buffers(cam_count);
  std::vector<std::mutex> mutexes(cam_count);
  std::vector<std::thread> threads;

  auto global_start = std::chrono::steady_clock::now();

  for (int i = 0; i < cam_count; ++i) {
    threads.emplace_back(camera_reader, i, std::ref(buffers[i]),
                         std::ref(mutexes[i]), global_start);
  }

  for (auto& t : threads) {
    t.join();
  }

  for (int i = 0; i < cam_count; ++i) {
    if (!buffers[i].empty()) {
      std::cout << "Cam " << (i + 1) << " original timestamps:\t";
      for (size_t j = 0; j < std::min<size_t>(4, buffers[i].size()); ++j) {
        std::cout << buffers[i][j].timestamp << "ms ";
      }
      std::cout << std::endl;
    } else {
      std::cout << "Cam" << (i + 1) << " buffer is empty." << std::endl;
    }
  }

  const auto synced = synchronize_buffers(buffers);
  std::cout << std::endl;

  for (int i = 0; i < cam_count; ++i) {
    if (!synced[i].empty()) {
      std::cout << "Cam " << (i + 1) << " aligned timestamps:\t";
      for (size_t j = 0; j < std::min<size_t>(4, synced[i].size()); ++j) {
        std::cout << synced[i][j].timestamp << "ms ";
      }
      std::cout << std::endl;
    } else {
      std::cout << "Cam" << (i + 1) << " aligned buffer is empty." << std::endl;
    }
  }

  return 0;
}