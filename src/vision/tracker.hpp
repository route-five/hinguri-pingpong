//
// Created by Hyunseung Ryu on 2025. 7. 8..
//

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "../utils/constants.hpp"
#include "contour.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <utility>

// class Tracker {
// protected:
//     const cv::Scalar lower_color_bound;
//     const cv::Scalar upper_color_bound;
//     cv::Mat prev_frame;
//     cv::Mat frame;
//     cv::Mat moving_mask;
//     cv::Mat moving_color_mask;
//
// public:
//     explicit Tracker(
//         cv::Scalar lower_color_bound,
//         cv::Scalar upper_color_bound
//     ) : lower_color_bound{std::move(lower_color_bound)},
//         upper_color_bound{std::move(upper_color_bound)} {
//     }
//
//     void update(const cv::Mat& new_frame) {
//         assert(!new_frame.empty() && "Frame cannot be empty - at Tracker::constructor");
//
//         frame = new_frame;
//
//         // 배경 제거 (subtraction)
//         if (!prev_frame.empty()) {
//             cv::Mat diff;
//             cv::absdiff(frame, prev_frame, diff);
//             cv::cvtColor(diff, moving_mask, cv::COLOR_BGR2GRAY);
//             cv::threshold(moving_mask, moving_mask, 30, 255, cv::THRESH_BINARY);
//         }
//         else {
//             moving_mask = cv::Mat::zeros(frame.size(), CV_8UC1);
//         }
//         frame.copyTo(prev_frame);
//
//         // morphology로 노이즈 제거
//         cv::erode(moving_mask, moving_mask, cv::Mat(), cv::Point(-1, -1), 1);
//         cv::dilate(moving_mask, moving_mask, cv::Mat(), cv::Point(-1, -1), 2);
//
//         // frame에서 움직이는 부분만 추출
//         cv::Mat moving_part;
//         cv::bitwise_and(frame, frame, moving_part, moving_mask);
//
//         // 색상 마스크 생성
//         cv::Mat hsv;
//         cv::cvtColor(moving_part, hsv, cv::COLOR_BGR2HSV);
//
//         // 움직이는 부분의 색상 마스크 생성
//         moving_color_mask.release();
//         cv::inRange(hsv, lower_color_bound, upper_color_bound, moving_color_mask);
//     }
//
//     friend Tracker& operator<<(Tracker& tracker, const cv::Mat& new_frame) noexcept {
//         tracker.update(new_frame);
//         return tracker;
//     }
//
//     [[nodiscard]] const cv::Mat& get_frame() const noexcept {
//         return frame;
//     }
//
//     [[nodiscard]] const cv::Mat& get_moving_mask() const noexcept {
//         return moving_mask;
//     }
//
//     [[nodiscard]] const cv::Mat& get_moving_color_mask() const noexcept {
//         return moving_color_mask;
//     }
//
//     [[nodiscard]] std::vector<Contour> find_contours() const {
//         // moving_color_mask가 초기화되지 않았거나 비어있는 경우 체크
//         if (moving_color_mask.empty()) {
//             std::cerr << "Warning: moving_color_mask is empty. Call update() first." << std::endl;
//             return std::vector<Contour>{};
//         }
//
//         // 데이터 타입 체크 (8비트 단일 채널이어야 함)
//         if (moving_color_mask.type() != CV_8UC1) {
//             std::cerr << "Warning: moving_color_mask has wrong type. Expected CV_8UC1, got: "
//                 << moving_color_mask.type() << std::endl;
//             return std::vector<Contour>{};
//         }
//
//         std::vector<std::vector<cv::Point>> contour_list;
//         try {
//             cv::findContours(moving_color_mask, contour_list, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//         }
//         catch (const cv::Exception& e) {
//             std::cerr << "OpenCV Error in findContours: " << e.what() << std::endl;
//             return std::vector<Contour>{};
//         }
//
//         std::vector<Contour> contours;
//         contours.reserve(contour_list.size());
//         for (const auto& c : contour_list) {
//             contours.emplace_back(c);
//         }
//         return contours;
//     }
//
//     static std::optional<Contour> most_circular_contour(
//         const std::vector<Contour>& contours,
//         const double threshold = CIRCULARITY_THRESHOLD
//     ) {
//         std::optional<Contour> most_contour = std::nullopt;
//
//         for (const auto& contour : contours) {
//             if (contour.empty() || contour.zero())
//                 continue;
//
//             const double dist = contour.distance_circularity(threshold);
//             if (dist < threshold && (!most_contour.has_value() || most_contour->distance_circularity() > dist)) {
//                 most_contour = contour;
//             }
//         }
//
//         return most_contour;
//     }
//
//     static std::optional<Contour> largest_contour(const std::vector<Contour>& contours) {
//         std::optional<Contour> largest_contour = std::nullopt;
//
//         for (const auto& contour : contours) {
//             if (contour.empty() || contour.zero())
//                 continue;
//
//             if (!largest_contour.has_value() || largest_contour->area() < contour.area()) {
//                 largest_contour = contour;
//             }
//         }
//
//         return largest_contour;
//     }
//
//     /**
//      * @brief 타겟의 위치를 카메라 좌표계에서 찾습니다.
//      * @return 가장 원형에 가까운 컨투어의 (최소 외접원 중심 좌표, 반지름)
//      */
//     [[nodiscard]] std::optional<std::pair<cv::Point2f, double>> get_camera_pos() const {
//         // moving_color_mask가 초기화되지 않은 경우 체크
//         if (moving_color_mask.empty()) {
//             std::cerr << "Warning: Cannot get camera position. Call update() first." << std::endl;
//             return std::nullopt;
//         }
//
//         const std::vector<Contour> contours = find_contours();
//
//         if (const auto most_contour = most_circular_contour(contours)) {
//             try {
//                 const auto ret = most_contour->min_enclosing_circle();
//                 if (ret.second > RADIUS_MIN) {
//                     return ret;
//                 }
//             }
//             catch (const std::exception& e) {
//                 std::cerr << "Error in min_enclosing_circle: " << e.what() << std::endl;
//                 return std::nullopt;
//             }
//         }
//
//         return std::nullopt;
//     }
// };

class Tracker {
protected:
    const cv::Scalar lower_color_bound;
    const cv::Scalar upper_color_bound;
    cv::Mat frame;
    cv::Mat hsv; // reused HSV buffer
    cv::Mat color_mask; // reused color mask

public:
    explicit Tracker(
        cv::Scalar lower_color_bound,
        cv::Scalar upper_color_bound
    ) : lower_color_bound(std::move(lower_color_bound)),
        upper_color_bound(std::move(upper_color_bound)) {
    }

    void update(const cv::Mat& new_frame) {
        assert(!new_frame.empty() && "Frame cannot be empty - at Tracker::update");
        // Only copy header (shallow)
        frame = new_frame;

        // Ensure buffers are the right size, reuse existing capacity
        if (hsv.size() != frame.size()) hsv.create(frame.size(), CV_8UC3);
        if (color_mask.size() != frame.size()) color_mask.create(frame.size(), CV_8UC1);

        // Convert BGR to HSV and apply color threshold
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, lower_color_bound, upper_color_bound, color_mask);
    }

    friend Tracker& operator<<(Tracker& tracker, const cv::Mat& new_frame) noexcept {
        tracker.update(new_frame);
        return tracker;
    }

    [[nodiscard]] const cv::Mat& get_frame() const noexcept {
        return frame;
    }

    [[nodiscard]] const cv::Mat& get_color_mask() const noexcept {
        return color_mask;
    }

    /**
     * @brief Computes the centroid of the largest-area contour in the current color mask
     * @return Optional centroid point, or nullopt if no valid contour found
     */
    [[nodiscard]] std::optional<cv::Point2f> get_camera_pos() const {
        if (color_mask.empty()) {
            return std::nullopt;
        }

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(color_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        if (contours.empty()) {
            return std::nullopt;
        }

        // Find the largest contour by area
        double max_area = 0.0;
        int best_idx = -1;
        for (int i = 0; i < (int)contours.size(); ++i) {
            double area = cv::contourArea(contours[i]);
            if (area > max_area) {
                max_area = area;
                best_idx = i;
            }
        }
        if (best_idx < 0) {
            return std::nullopt;
        }

        // Compute centroid via moments
        const auto& cnt = contours[best_idx];
        cv::Moments m = cv::moments(cnt);
        if (m.m00 <= 0.0) {
            return std::nullopt;
        }
        return cv::Point2f(static_cast<float>(m.m10 / m.m00),
                           static_cast<float>(m.m01 / m.m00));
    }
};

#endif // TRACKER_HPP
