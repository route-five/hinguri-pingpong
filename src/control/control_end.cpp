#define _CRT_SECURE_NO_WARNINGS
#include <thread>
#include <mutex>
#include <optional>
#include <atomic>
#include <iostream>
#include <cstdio>

#include "dynamixel_actuator.hpp"
#include "linear_actuator.hpp"
#include "../vision/bridge.hpp"
#include "../utils/constants.hpp"

/*constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;*/

class ControlEnd {
private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const Bridge::Payload& payload, std::atomic<bool>& execute_done) {
        const auto& [x, theta, swing_start, swing_end, use_right_hand] = payload;

        // 1) Move the linear actuator
        linearActuator.move_actu(x);

        // 2) Move to swing_start and wait
        bool swing_start_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_start},
            &swing_start_done
        );
        Log::debug(std::format("swing start done - success: {}", swing_start_done));
        // swing_start_done == true here

        // 3) Only after swing_start is done, move to swing_end and wait
        bool swing_end_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_end},
            &swing_end_done
        );
        Log::debug(std::format("swing end done - success: {}", swing_end_done));
        // swing_end_done == true here

        execute_done = true;
    }

    void shutdown() {
        Bridge::Payload init{TABLE_WIDTH / 2, 90, 90, 90};
        actuators.bulk_move_by_degrees({init.theta, init.swing_start});
        linearActuator.move_actu(init.x);
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }

public:
    ControlEnd() : actuators({MID_SERIAL_PORT, BOT_SERIAL_PORT}) {
    }

    ~ControlEnd() {
        shutdown();
    }

    void run(
        std::queue<Bridge::Payload>& queue,
        std::mutex& queue_mutex,
        std::condition_variable& queue_push_flag,
        std::atomic<bool>& stop,
        std::atomic<bool>& execute_done
    ) {
        if (!actuators.initialize()) {
            Log::error("[ControlEnd::run] Failed to initialize actuators.");
            stop = true;
            return;
        }

        while (true) {
            std::unique_lock lock(queue_mutex);
            queue_push_flag.wait(lock, [&] {
                return !queue.empty() || stop;
            });

            if (!queue.empty()) {
                Bridge::Payload payload = queue.front();
                queue.pop();
                lock.unlock();

                Log::debug(std::format(
                    "[ControlEnd::run] Received Payload with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
                    payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
                execute(payload, execute_done);
            }
            else if (stop) {
                lock.unlock();

                Log::info("[ControlEnd::run] Stopping as stop signal received.");
                break;
            }
        }
    }
};

int main() {
    ControlEnd control_end;

    std::atomic stop = false;

    std::queue<Bridge::Payload> queue;
    std::mutex queue_mutex;
    std::condition_variable queue_push_flag;

    std::atomic execute_done = true;
    // std::mutex execute_done_mutex;
    // std::condition_variable execute_done_flag;

    auto consumer = std::thread(
        &ControlEnd::run, &control_end,
        std::ref(queue),
        std::ref(queue_mutex),
        std::ref(queue_push_flag),
        std::ref(stop),
        std::ref(execute_done)
    );

    while (true) {
        std::lock_guard lock(queue_mutex);
        if (execute_done.load() && queue.empty()) {
            //cv::Point3f target_pos;
            //cv::Vec3f target_speed;
            //float target_angle;

            //std::printf("Enter target pos (x, y, z): ");
            //std::scanf("%f %f %f", &target_pos.x, &target_pos.y, &target_pos.z);
            //if (target_pos.x <= -1 || target_pos.y <= -1 || target_pos.z <= -1) { // escape
            //    stop = true;
            //    queue_push_flag.notify_all();
            //    break;
            //}

            float swing_start, swing_end;
            std::printf("Enter swing start and end: ");
            std::scanf("%f %f", &swing_start, &swing_end);
            if (swing_start == -1 && swing_end == -1) {
                stop = true;
                queue_push_flag.notify_all();
                break;
            }

            Bridge::Payload payload = {0.f, 30.f, swing_start, swing_end, true};
            queue.push(payload);
            queue_push_flag.notify_one();
            execute_done = false;

            Log::info(std::format("[main] Queue: {} items.", queue.size()));
        }
    }

    consumer.join();

    return 0;
}
