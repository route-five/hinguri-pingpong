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
        const int thickness = 2,
        const double font_scale = 1.0
    ) {
        cv::putText(frame, text, position, cv::FONT_HERSHEY_SIMPLEX, font_scale, color, thickness, cv::LINE_AA);
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
