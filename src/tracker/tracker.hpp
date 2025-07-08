//
// Created by Hyunseung Ryu on 2025. 7. 8..
//

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <opencv2/opencv.hpp>

#include "../constants.hpp"

namespace Tracker {

using Contour = std::vector<cv::Point>;

class AbstractTracker {
protected:
  cv::Mat frame;

public:
  explicit AbstractTracker(const cv::Mat &frame) : frame(frame) {}

  void set_mask(cv::Mat &mask) const {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);
  }

  void find_contours(cv::Mat &mask, std::vector<Contour> &contours) const {
    set_mask(mask);
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
  }

  // AbstractTracker class can be extended with methods for tracking objects
  // For example, methods to initialize tracking, update positions, etc.
  // virtual void initialize() = 0;
  // virtual void update() = 0;
  // virtual void reset() = 0;
  //
  // virtual ~AbstractTracker() =
  //     default; // Virtual destructor for proper cleanup of derived classes
};
} // namespace Tracker

#endif // TRACKER_HPP
