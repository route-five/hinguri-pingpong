#include "control/dynamixel_actuator.hpp"
#include "control/linear_actuator.hpp"
#include "vision/bridge.hpp"
#include "utils/constants.hpp"
#include <thread>
#include <mutex>
#include <optional>
#include <atomic>
#include <iostream>_

inline std::mutex mutex;
inline std::atomic<bool> stop_all{false};
inline std::optional<Bridge::Payload> shared_payload;

constexpr int TOP_MOTOR_ID = 4;
constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;

class ControlEnd {
public:
    ControlEnd() : actuators({TOP_MOTOR_ID, MID_MOTOR_ID, BOT_MOTOR_ID}) {
    }

    ~ControlEnd() {
        shutdown();
    }

    [[noreturn]] void run() {
        if (!actuators.initialize()) {
            std::cerr << "Failed to initialize actuators" << std::endl;
            return;
        }
        while (!stop_all.load()) {
            Bridge::Payload payload;
            {
                std::lock_guard lock(mutex);
                if (!shared_payload.has_value())
                    continue;

                payload = shared_payload.value();
                shared_payload.reset();

                std::cout << "Received from vision:\n"
                    << "\tx: " << payload.x << " cm\n"
                    << "\ttheta: " << payload.theta << " deg\n"
                    << "\tswing_start: " << payload.swing_start << " deg\n"
                    << "\tswing_end: " << payload.swing_end << " deg\n"
                    << "\twrist_angle: " << payload.wrist_angle << " deg\n"
                    << "=========================\n";
            }

            execute(payload);
        }

        shutdown();
    }

private:
    BulkDynamixelActuator actuators;
    LinearActuator linearActuator;

    void execute(const Bridge::Payload& payload) {
        const auto& [x, theta, swing_start, swing_end, wrist_angle, use_right_hand] = payload;

        const float top_target = wrist_angle;
        const float mid_target = theta;
        // 0) Move the linear actuator
        linearActuator.move_actu(x);

        // 1) Move to swing_start and wait
        bool swing_start_done = false;
        actuators.move_and_wait_by_degrees(
            {top_target, mid_target, swing_start},
            &swing_start_done
        );
        // swing_start_done == true here


        // 2) Only after swing_start is done, move to swing_end and wait
        bool swing_end_done = false;
        actuators.move_and_wait_by_degrees(
            {top_target, mid_target, swing_end},
            &swing_end_done
        );
        // swing_end_done == true here
    }

    void shutdown() {
        Bridge::Payload init{TABLE_WIDTH / 2, 90, 90, 90, 90};
        actuators.bulk_move_by_degrees({init.wrist_angle, init.theta, init.swing_start});
        linearActuator.move_actu(init.x);
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }
};

int main() {
    ControlEnd control_end;
    std::thread control_thread(&ControlEnd::run, &control_end);

    /*while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        float x, theta, swing_start, swing_end, wrist_angle;
        std::cout << "x: ";           std::cin >> x;
        if (x == -1000) { stop_all = true; break; }

        std::cout << "theta: ";      std::cin >> theta;
        std::cout << "swing_start: "; std::cin >> swing_start;
        std::cout << "swing_end: ";  std::cin >> swing_end;
        std::cout << "wrist_angle: "; std::cin >> wrist_angle;

        shared_payload = Bridge::Payload{ x, theta, swing_start, swing_end, wrist_angle };
    }*/

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        cv::Point3f target_pos = {};
        cv::Vec3f target_speed = {};
        float target_angle;

        std::cout << "Enter target pos (x, y, z): ";
        std::cin >> target_pos.x >> target_pos.y >> target_pos.z;
        if (target_pos.x <= -1 || target_pos.y <= -1 || target_pos.z <= -1) { // escape
            stop_all = true;
            break;
        }

        std::cout << "Enter target speed (v_x, v_y, v_z): ";
        std::cin >> target_speed[0] >> target_speed[1] >> target_speed[2];
        std::cout << "Enter target angle (deg, from ground, 0~90): ";
        std::cin >> target_angle;

        shared_payload = Bridge::convert(target_pos, target_speed, target_angle);
    }

    control_thread.join();
    return 0;
}
