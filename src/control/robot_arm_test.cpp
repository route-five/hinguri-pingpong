#define _CRT_SECURE_NO_WARNINGS
#include <thread>
#include <mutex>
#include "dynamixel_actuator.hpp"
#include "linear_actuator.hpp"
#include "robot_arm_controller.hpp"
#include "vision/bridge.hpp"
#include "utils/constants.hpp"

class ControlEnd {
public:
    RobotArmController arm_controller_;
    LinearActuator linear_actuator_;

    float last_x = TABLE_WIDTH / 2;

    void execute(const Bridge::Payload& payload) {
        const auto& [x, steps] = payload;

        //1) Move the linear actuator
        linear_actuator_.move_actu(x);
        const float dt_linear_ms = std::abs(x - last_x) / LINEAR_VELOCITY * 1000; // FIXME: LINEAR_VELOCITY 다시 재기
        last_x = x;

        Log::debug(std::format("Linear Actuator dt: {}s", dt_linear_ms / 1000));
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt_linear_ms)));

        //2) Move the arm
        arm_controller_.execute(steps);
    }

    void shutdown() {
        linear_actuator_.move_actu(TABLE_WIDTH / 2);
        arm_controller_.execute_each({ 90, 120, -80, 50 });
        if (sharedPortInitialized)
            sharedPortHandler->closePort();
    }

    ControlEnd() = default;

    ~ControlEnd() {
        shutdown();
    }
};

int main() {
    ControlEnd control_end;

    while (true) {
        float x;
        std::printf("Enter x: ");
        std::scanf("%f", &x);

        const auto payload = Bridge::actions({ x == -1 ? TABLE_WIDTH / 2 : x, 0, 0 });
        control_end.execute(payload);

        if (x == -1) {
            Log::debug(Log::blue("done"));
            break;
        }
    }

    return 0;
}
