#pragma once
#include <chrono>
#include <thread>

inline void sleep_ms(const unsigned int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void sleep_s(const unsigned int s) {
  std::this_thread::sleep_for(std::chrono::seconds(s));
}