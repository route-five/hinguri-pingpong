//
// Created by Hyunseung Ryu on 2025. 7. 10..
//

#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

int main() {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cerr << "Webcam open failed!" << std::endl;
    return -1;
  }

  cv::Mat frame;
  cap.read(frame);

  // ROI 선택
  cv::Rect bbox = cv::selectROI("Select Orange Ball", frame, false, false);
  cv::destroyWindow("Select Orange Ball");

  // CSRT 트래커 생성
  cv::Ptr<cv::Tracker> tracker = cv::TrackerCSRT::create();
  tracker->init(frame, bbox);

  while (cap.read(frame)) {
    if (frame.empty())
      continue;

    bool success = tracker->update(frame, bbox);

    if (success) {
      cv::rectangle(frame, bbox, cv::Scalar(255, 0, 0), 2);
      cv::putText(frame, "CSRT Tracking", cv::Point(20, 40),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);
    } else {
      cv::putText(frame, "Tracking Failure", cv::Point(20, 40),
                  cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    cv::imshow("CSRT Tracker", frame);
    if (cv::waitKey(1) == 'q')
      break;
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}