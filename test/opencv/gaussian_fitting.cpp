#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

double gaussian(double x, double A, double mu, double sigma) {
  return A * exp(-0.5 * pow((x - mu) / sigma, 2.0));
}

// 매우 단순한 brute-force Gaussian fit (최적화는 생략)
int find_max_gaussian_peak(const std::vector<float> &profile) {
  double best_mu = 0, best_score = -1;

  for (double mu = 5; mu < profile.size() - 5; mu += 0.5) {
    double score = 0;
    for (int i = 0; i < profile.size(); ++i) {
      double val = gaussian(i, 1.0, mu, 3.0);
      score += val * profile[i];
    }
    if (score > best_score) {
      best_score = score;
      best_mu = mu;
    }
  }

  return static_cast<int>(best_mu);
}

int main() {
  // cv::VideoCapture cap(0);
  // if (!cap.isOpened()) {
  //   std::cerr << "Cannot open camera.\n";
  //   return -1;
  // }

  while (true) {
    cv::Mat frame, gray;
    // cap >> frame;
    frame = cv::imread("img/test2/original/20250701_173805_150.png");

    if (frame.empty())
      break;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // 관심 영역 (ROI) 설정 — 정 중앙에 세로로 긴 박스
    int cx = frame.cols / 2;
    cv::Rect roi(cx - 5, 0, 10, frame.rows);
    cv::Mat roi_gray = gray(roi);

    // y 방향 평균 intensity 추출 (1D profile)
    std::vector<float> profile(roi_gray.rows, 0);
    for (int y = 0; y < roi_gray.rows; ++y) {
      for (int x = 0; x < roi_gray.cols; ++x)
        profile[y] += roi_gray.at<uchar>(y, x);
      profile[y] /= roi_gray.cols;
    }

    // Gaussian fitting으로 peak 중심 추정
    int peak_y = find_max_gaussian_peak(profile);
    cv::Point center(cx, peak_y);

    // 시각화
    cv::circle(frame, center, 8, cv::Scalar(255, 0, 255), -1); // 보라색 점
    cv::rectangle(frame, roi, cv::Scalar(255, 255, 0), 1);
    cv::imshow("Gaussian Fitting Center", frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

  // cap.release();
  cv::destroyAllWindows();
  return 0;
}