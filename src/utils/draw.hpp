//
// Created by Hyunseung Ryu on 2025. 7. 28..
//

#ifndef DRAW_HPP
#define DRAW_HPP

#include <opencv2/opencv.hpp>

#include "constants.hpp"

namespace Draw {
    inline void put_text(
        cv::Mat& frame,
        const std::string& text,
        const cv::Point& position,
        const cv::Scalar& color = COLOR_BLACK,
        const int thickness = 1,
        const double font_scale = 0.5
    ) {
        cv::putText(frame, text, position, cv::FONT_HERSHEY_SIMPLEX, font_scale, color, thickness, cv::LINE_AA);
    }

    inline void put_text_border(
        cv::Mat& frame,
        const std::string& text,
        const cv::Point& position,
        const cv::Scalar& color = COLOR_WHITE,
        const int thickness = 1,
        const double font_scale = 0.5
    ) {
        cv::putText(frame, text, position, cv::FONT_HERSHEY_SIMPLEX, font_scale, COLOR_BLACK, thickness + 2,
                    cv::LINE_AA);
        cv::putText(frame, text, position, cv::FONT_HERSHEY_SIMPLEX, font_scale, color, thickness, cv::LINE_AA);
    }

    inline void put_circle(
        cv::Mat& frame,
        const cv::Point2f& center,
        const int radius,
        const cv::Scalar& color = COLOR_MAGENTA,
        const int thickness = -1
    ) {
        cv::circle(frame, center, radius, color, thickness, cv::LINE_AA);
    }

    inline void put_arrow(
        cv::Mat& frame,
        const cv::Point2f& start,
        const cv::Point2f& end,
        const cv::Scalar& color = COLOR_GREEN,
        const int thickness = 1,
        const double tip_length = 0.3
    ) {
        cv::arrowedLine(frame, start, end, color, thickness, cv::LINE_AA, 0, tip_length);
    }

    inline std::string to_string(const std::string& prefix, const cv::Point2f& pt, const std::string& unit = "px") {
        return std::format("{0}: [{1:+.1f}{3}, {2:+.1f}{3}]", prefix, pt.x, pt.y, unit);
    }

    inline std::string to_string(const std::string& prefix, const cv::Vec2f& vec, const std::string& unit = "px") {
        return std::format("{0}: [{1:+.1f}{3}, {2:+.1f}{3}]", prefix, vec[0], vec[1], unit);
    }

    inline std::string to_string(const std::string& prefix, const cv::Point3f& pt, const std::string& unit = "cm") {
        return std::format("{0}: [{1:+.1f}{4}, {2:+.1f}{4}, {3:+.1f}{4}]", prefix, pt.x, pt.y, pt.z, unit);
    }

    inline std::string to_string(const std::string& prefix, const cv::Vec3f& vec, const std::string& unit = "cm") {
        return std::format("{0}: [{1:+.1f}{4}, {2:+.1f}{4}, {3:+.1f}{4}]", prefix, vec[0], vec[1], vec[2], unit);
    }
}

#endif //DRAW_HPP
