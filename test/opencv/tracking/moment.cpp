#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../../src/constants.hpp"
#include "../../../src/stream/webcam_video_stream.hpp"
#include "../../../src/tracker/tracker.hpp"

int main() {
  WebcamVideoStream stream({});
  if (!stream.is_opened()) {
    std::cerr << "카메라를 열 수 없습니다.\n";
    return -1;
  }

  stream.start();

  while (true) {
    cv::Mat frame = stream.read();
    if (frame.empty())
      continue;

    Tracker tracker{frame};

    tracker.set_color_mask(ORANGE_MIN, ORANGE_MAX);
    const std::vector<Contour> contours = tracker.find_contours();

    // draw contours and enclosing circles
    for (const auto &contour : contours) {
      if (contour.empty())
        continue;

      contour.draw(frame, COLOR_RED);

      auto [center, radius] = contour.min_enclosing_circle();
      cv::circle(frame, center, static_cast<int>(radius), COLOR_BLUE, 2,
                 cv::LINE_AA);

      if (contour.area() != 0) {
        auto moment = contour.moment();
        cv::circle(frame, moment, 2, COLOR_GREEN, 3, cv::LINE_AA);
      }
    }

    // Draw legend in the bottom-left corner
    const int base_line = frame.rows - 10;
    constexpr int font = cv::FONT_HERSHEY_SIMPLEX;
    constexpr double scale = 1;
    constexpr int thickness = 1;

    cv::putText(frame,
                std::format("FPS: {:.1f}/{}", stream.get_fps(),
                            stream.get_prop(cv::CAP_PROP_FPS)),
                cv::Point(10, base_line), font, scale, COLOR_BLACK, thickness,
                cv::LINE_AA);

    cv::imshow("Moment", frame);

    if (cv::waitKey(1) == 'q')
      break;
  }

  stream.stop();
  cv::destroyAllWindows();
  return 0;
}