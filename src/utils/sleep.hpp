#pragma once
#include <thread>
#include <chrono>

void sleep_ms(unsigned int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void sleep_s(unsigned int s) {
	std::this_thread::sleep_for(std::chrono::seconds(s));
}