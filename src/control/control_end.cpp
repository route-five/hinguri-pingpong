#include <thread>
#include <mutex>
#include <optional>
#include <atomic>
#include <iostream>_

#include "dynamixel_actuator.hpp"
#include "linear_actuator.hpp"
#include "../vision/bridge.hpp"
#include "../utils/constants.hpp"

inline std::mutex mutex;
inline std::atomic<bool> stop_all{false};
inline std::optional<Bridge::Payload> shared_payload;

/*constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;*/

class ControlEnd {
private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const Bridge::Payload& payload) {
        const auto& [x, theta, swing_start, swing_end, use_right_hand] = payload;

        // 1) Move the linear actuator
        linearActuator.move_actu(x);

        // 2) Move to swing_start and wait
        bool swing_start_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_start},
            &swing_start_done
        );
        // swing_start_done == true here

        // 3) Only after swing_start is done, move to swing_end and wait
        bool swing_end_done = false;
        actuators.move_and_wait_by_degrees(
            {theta, swing_end},
            &swing_end_done
        );
        // swing_end_done == true here
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
        std::condition_variable& flag,
        const std::atomic<bool>& stop
    ) {
        if (!actuators.initialize()) {
            Log::error("[ControlEnd::run] Failed to initialize actuators.");
            return;
        }

        while (true) {
            std::unique_lock lock(queue_mutex);
            flag.wait(lock, [&] {
                return !queue.empty() || stop;
            });

            if (!queue.empty()) {
                Bridge::Payload payload = queue.front();
                queue.pop();
                lock.unlock();

                Log::debug(std::format(
                    "[ControlEnd::run] Received Payload with x: {}, theta: {}, swing_start: {}, swing_end: {}, use_right_hand: {}",
                    payload.x, payload.theta, payload.swing_start, payload.swing_end, payload.use_right_hand));
                execute(payload);
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

    std::queue<Bridge::Payload> queue;
    std::mutex queue_mutex;
    std::condition_variable flag;
    std::atomic stop = false;

    auto consumer = std::thread(
        &ControlEnd::run, &control_end,
        std::ref(queue),
        std::ref(queue_mutex),
        std::ref(flag),
        std::ref(stop)
    );

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        cv::Point3f target_pos;
        cv::Vec3f target_speed;
        float target_angle;

        std::cout << "Enter target pos (x, y, z): ";
        std::cin >> target_pos.x >> target_pos.y >> target_pos.z;
        if (target_pos.x <= -1 || target_pos.y <= -1 || target_pos.z <= -1) { // escape
            stop_all = true;
            break;
        }

        // std::cout << "Enter target speed (v_x, v_y, v_z): ";
        // std::cin >> target_speed[0] >> target_speed[1] >> target_speed[2];
        // std::cout << "Enter target angle (deg, from ground, 0~90): ";
        // std::cin >> target_angle;

        shared_payload = Bridge::convert(target_pos, {1.f, 1.f, 1.f}, 30);
    }

    consumer.join();

    return 0;
}
