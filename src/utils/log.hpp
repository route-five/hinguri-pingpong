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
}

#endif //LOG_HPP
