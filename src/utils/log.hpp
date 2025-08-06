//
// Created by Hyunseung Ryu on 2025. 8. 3..
//

#ifndef LOG_HPP
#define LOG_HPP

#include <print>
#include "constants.hpp"

namespace Log {
    inline void info(const std::string& message) {
        std::print("{}{}{}\n", ANSI_CYAN, message, ANSI_RESET);
    }

    inline void warn(const std::string& message) {
        std::print("{}{}{}\n", ANSI_BRIGHT_YELLOW, message, ANSI_RESET);
    }

    inline void error(const std::string& message) {
        std::print("{}{}{}\n", ANSI_BRIGHT_WHITE, message, ANSI_RESET);
    }

    inline void debug(const std::string& message) {
        std::print("{}{}{}\n", ANSI_WHITE, message, ANSI_RESET);
    }

    inline std::string blue(const std::string& message) {
        return std::format("{}{}{}", ANSI_BRIGHT_BLUE, message, ANSI_RESET);
    }

    inline std::string green(const std::string& message) {
        return std::format("{}{}{}", ANSI_BRIGHT_GREEN, message, ANSI_RESET);
    }

    inline std::string red(const std::string& message) {
        return std::format("{}{}{}", ANSI_BRIGHT_RED, message, ANSI_RESET);
    }

    inline std::string yellow(const std::string& message) {
        return std::format("{}{}{}", ANSI_BRIGHT_YELLOW, message, ANSI_RESET);
    }

    inline std::string magenta(const std::string& message) {
        return std::format("{}{}{}", ANSI_BRIGHT_MAGENTA, message, ANSI_RESET);
    }

    inline std::string to_string(const cv::Point3f& point) {
        return std::format("({:.2f}, {:.2f}, {:.2f})", point.x, point.y, point.z);
    }

    inline std::string to_string(const cv::Vec3f& vec) {
        return std::format("[{:.2f}, {:.2f}, {:.2f}]", vec[0], vec[1], vec[2]);
    }
}

#endif //LOG_HPP
