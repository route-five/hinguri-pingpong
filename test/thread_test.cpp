//
// Created by Hyunseung Ryu on 2025. 8. 3..
//

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "../src/utils/log.hpp"
#include "../src/vision/bridge.hpp"

class Producer {
public:
    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& mutex,
        std::condition_variable& condition,
        std::atomic<bool>& stop
    ) const {
        float i = 0;
        while (!stop.load()) {
            {
                std::lock_guard lock(mutex);
                queue.push(Bridge::convert({i, i, i}, {i, i, i}, i));
            }
            condition.notify_one();

            Log::info(std::format(
                "Producer: Payload set with x: {}, theta: {}, swing_start: {}, swing_end: {}, wrist_angle: {}, use_right_hand: {}",
                i, i, i, i, i, i));

            i++;
            if (i >= 4) { // Stop after 4 iterations
                stop = true;
                condition.notify_all();
                Log::debug("Producer: Stopping after 4 iterations.");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

class Consumer {
public:
    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& mutex,
        std::condition_variable& condition,
        std::atomic<bool>& stop
    ) const {
        while (true) {
            std::unique_lock lock(mutex);
            condition.wait(lock, [&] {
                return !queue.empty() || stop;
            });

            if (!queue.empty()) {
                Bridge::Payload payload = queue.front();
                queue.pop();
                lock.unlock();

                Log::info(std::format(
                    "Consumer: Received Payload with x: {}, theta: {}, swing_start: {}, swing_end: {}, wrist_angle: {}, use_right_hand: {}",
                    payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.wrist_angle, payload.use_right_hand));
            }
            else if (stop) {
                lock.unlock();
                break;
            }
        }
    }
};

int main() {
    Producer producer;
    Consumer consumer;

    std::queue<Bridge::Payload> queue;
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic stop = false;

    auto producer_thread = std::thread(&Producer::run, &producer, std::ref(queue), std::ref(mutex), std::ref(condition), std::ref(stop));
    auto consumer_thread = std::thread(&Consumer::run, &consumer, std::ref(queue), std::ref(mutex), std::ref(condition), std::ref(stop));

    producer_thread.join();
    consumer_thread.join();

    return 0;
}
