#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../../src/stream/webcam_video_stream.hpp"
#include "../../../src/tracker/circularity_based.hpp"

int main() {
  WebcamVideoStream stream(0);
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  stream.start();

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty())
      continue;

    Tracker::CircularityBased tracker(frame);

    // mask
    cv::Mat mask;
    tracker.set_mask(mask);

    // find contours
    std::vector<Tracker::Contour> contours;
    tracker.find_contours(mask, contours);

    if (contours.empty())
      continue;

    // draw contours and enclosing circles
    for (int i = 0; i < contours.size(); ++i) {
      cv::drawContours(frame, contours, i, cv::Scalar(0, 0, 255), 2,
                       cv::LINE_AA);

      auto [center, radius] = tracker.min_enclosing_circle(contours[i]);
      cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(255, 0, 0),
                 2, cv::LINE_AA);
    }

    // draw the most circular contour
    Tracker::Contour most_contour = tracker.most_circular_contour(contours);
    auto [center, radius] = tracker.min_enclosing_circle(most_contour);

    cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 255, 0),
               2, cv::LINE_AA);

    // draw legend in the bottom-left corner
    const int base_line = frame.rows - 60;
    constexpr int font = cv::FONT_HERSHEY_SIMPLEX;
    constexpr double scale = 0.6;
    constexpr int thickness = 1;

    cv::putText(frame, "Green: Most Circular One", cv::Point(10, base_line),
                font, scale, cv::Scalar(0, 255, 0), thickness, cv::LINE_AA);
    cv::putText(frame, "Blue: Enclosing Contour Circles",
                cv::Point(10, base_line + 20), font, scale,
                cv::Scalar(255, 0, 0), thickness, cv::LINE_AA);
    cv::putText(frame, "Red: Every Contours", cv::Point(10, base_line + 40),
                font, scale, cv::Scalar(0, 0, 255), thickness, cv::LINE_AA);

    cv::imshow("Circularity Based Tracking", frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}