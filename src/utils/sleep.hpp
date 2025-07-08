#include <thread>
#include <chrono>

void sleep_ms(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void sleep_s(int s) {
	std::this_thread::sleep_for(std::chrono::seconds(s));
}