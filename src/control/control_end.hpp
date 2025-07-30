//
// Created by Hyunseung Ryu on 2025. 7. 30..
//

#ifndef CONTROL_END_HPP
#define CONTROL_END_HPP

#include "control/linear_actuator.hpp"
#include "vision/bridge.hpp"
#include "utils/constants.hpp"

constexpr float START_SWING = -90.0; // deg
constexpr int TOP_MOTOR_ID = 4;
constexpr int MID_MOTOR_ID = 3;
constexpr int BOT_MOTOR_ID = 2;

class ControlEnd {
public:
    ControlEnd()
        : actuators({TOP_MOTOR_ID, MID_MOTOR_ID, BOT_MOTOR_ID}) {
    }

    ~ControlEnd() {
        shutdown();
    }

    void run() {
        if (!actuators.initialize()) {
            std::cerr << "Failed to initialize actuators" << std::endl;
            return;
        }
        while (true) {
            if (!has_sent.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            BridgePayload input;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (!shared_payload.has_value()) continue;
                input = shared_payload.value();
                shared_payload.reset();
                std::cout << "(" << input.x << ", " << input.z << ") 로 받음" << std::endl;
                has_sent = false;
            }
            if (input.x == -1000) break;
            execute(input);
        }
        shutdown();
    }

private:
    BulkDynamixelActuator actuators{};
    LinearActuator linearActuator{};

    void execute(const BridgePayload& payload) {
        const auto& [x, theta, swing_length, wrist_angle, use_right_hand] = payload;

        const double hand_coeff = use_right_hand ? 1.0 : -1.0;

        double top_target = wrist_angle * hand_coeff;
        double mid_target = theta * hand_coeff;
        double bot_target = START_SWING * hand_coeff;
        actuators.bulk_move_by_degrees({top_target, mid_target, bot_target});
        linearActuator.move_actu(-x);

        bot_target = (START_SWING + swing_length) * hand_coeff;
        actuators.bulk_move_by_degrees({top_target, mid_target, bot_target});
    }

    void shutdown() {
        actuators.bulk_move_by_degrees({0, 0, 0});
        linearActuator.move_actu(0);
        sharedPortHandler->closePort();
    }
};

#endif //CONTROL_END_HPP
