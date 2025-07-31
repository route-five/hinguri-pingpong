#include "control/dynamixel_actuator.hpp"
#include "control/linear_actuator.hpp"
#include "vision/bridge.hpp"
#include "utils/constants.hpp"

inline std::mutex mutex;
inline std::atomic<bool> stop_all{ false };
inline std::optional<Bridge::Payload> shared_payload;

constexpr int TOP_MOTOR_ID = 4;
constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;

class ControlEnd {
public:
    ControlEnd()
        : actuators({ TOP_MOTOR_ID, MID_MOTOR_ID, BOT_MOTOR_ID }) {
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

                std::cout << "Received from vision: " << std::endl
                    << "\tx: " << payload.x << " cm" << std::endl
                    << "\ttheta: " << payload.theta << " deg" << std::endl
                    << "\tswing_start: " << payload.swing_start << " deg" << std::endl
                    << "\tswing_end: " << payload.swing_end << " deg" << std::endl
                    << "\twrist_angle: " << payload.wrist_angle << " deg" << std::endl
                    << "=========================" << std::endl;
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
        float bot_target = swing_start;

        actuators.bulk_move_by_degrees({ top_target, mid_target, bot_target });
        linearActuator.move_actu(x);

        bot_target = swing_end;
        actuators.bulk_move_by_degrees({ top_target, mid_target, bot_target });
    }

    void shutdown() {
        Bridge::Payload init{ TABLE_WIDTH / 2, 90, 90, 90, 90 };
        actuators.bulk_move_by_degrees({ init.wrist_angle, init.theta, init.swing_start });
        linearActuator.move_actu(init.x);
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }
};

int main() {
    ControlEnd control_end;
    std::thread control_thread(&ControlEnd::run, &control_end);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        float x, theta, swing_start, swing_end, wrist_angle;
        
        std::cout << "x: ";
        std::cin >> x;
        if (x == -1000) {
            stop_all = true;
            break;
        }

        std::cout << "theta: ";
        std::cin >> theta;

        std::cout << "swing_start: ";
        std::cin >> swing_start;
        
        std::cout << "swing_end: ";
        std::cin >> swing_end;

        std::cout << "wrist_angle: ";
        std::cin >> wrist_angle;
        
        Bridge::Payload payload(x, theta, swing_start, swing_end, wrist_angle);
        shared_payload = payload;
    }

    control_thread.join();

    return 0;
}