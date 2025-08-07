//
// Created by Hyunseung Ryu on 2025. 8. 8..
//

#ifndef HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP
#define HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP

#include <thread>
#include <vector>
#include "dynamixel_sdk.h"
#include "../utils/constants.hpp"
#include "../vision/bridge.hpp"
#include "dynamixel_actuator.hpp"

constexpr uint16_t LEN_GOAL_POSITION = 4; // Goal Position 길이 (4바이트)
constexpr uint8_t ADDR_PRESENT_MOVING = 122; // Moving 플래그 주소
constexpr uint16_t LEN_PRESENT_MOVING = 1; // Moving 플래그 길이

class RobotArmController {
private:
    std::array<int, 4> ids = DYNAMIXEL_IDS;
    dynamixel::PortHandler* portHandler = sharedPortHandler;
    dynamixel::PacketHandler* packetHandler = sharedPacketHandler;

public:
    RobotArmController() = default;

    [[nodiscard]] bool initialize() const {
        if (!sharedPortInitialized) {
            if (!portHandler->openPort()) {
                Log::error("[RobotArmController::initialize] Failed to open port: " + std::string(DEVICENAME));
                return false;
            }
            if (!portHandler->setBaudRate(BAUDRATE)) {
                Log::error("[RobotArmController::initialize] Failed to set baudrate: " + std::to_string(BAUDRATE));
                return false;
            }
            sharedPortInitialized = true;
        }
        for (int id : ids) {
            uint8_t err = 0;
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &err);
            int res = packetHandler->write1ByteTxRx(portHandler, id, ADDR_OPERATING_MODE,
                                                    POSITION_CONTROL_MODE, &err);
            if (res != COMM_SUCCESS || err) {
                Log::error(std::format("[RobotArmController::initialize] Failed to set operating mode for motor {}, error code: {}", id,
                                       err));
                return false;
            }
            res = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                                                TORQUE_ENABLE, &err);
            if (res != COMM_SUCCESS || err) {
                Log::error(std::format("[RobotArmController::initialize] Failed to enable torque for motor {}, error code: {}", id, err));
                return false;
            }
        }

        Log::info("[RobotArmController::initialize] All motors initialized successfully");
        return true;
    }

    bool execute_each(const Bridge::Step& step) {
        const auto& [bot_angle, mid_angle, top_angle, upper_angle] = step;

        // degree -> dynamixel unit 변환
        std::vector<int> target_units = {
            Bridge::to_dynamixel_unit_from_deg(bot_angle),
            Bridge::to_dynamixel_unit_from_deg(mid_angle),
            Bridge::to_dynamixel_unit_from_deg(top_angle),
            Bridge::to_dynamixel_unit_from_deg(upper_angle)
        };

        // GroupSyncWrite 로 4대의 Goal Position 동기 전송
        dynamixel::GroupSyncWrite syncWrite(
            portHandler, packetHandler,
            ADDR_GOAL_POSITION, LEN_GOAL_POSITION
        );

        for (size_t i = 0; i < ids.size(); ++i) {
            int unit = target_units[i];
            uint8_t param_goal[4];
            param_goal[0] = DXL_LOBYTE(DXL_LOWORD(unit));
            param_goal[1] = DXL_HIBYTE(DXL_LOWORD(unit));
            param_goal[2] = DXL_LOBYTE(DXL_HIWORD(unit));
            param_goal[3] = DXL_HIBYTE(DXL_HIWORD(unit));
            syncWrite.addParam(ids[i], param_goal);
        }
        syncWrite.txPacket();
        syncWrite.clearParam();

        // GroupBulkRead 로 PRESENT_POSITION 읽어오기
        dynamixel::GroupBulkRead bulkRead(portHandler, packetHandler);
        for (int id : ids) {
            bulkRead.addParam(id, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
        }

        // 모두 목표 위치에 도달할 때까지 폴링
        int try_count = 0;

        while (try_count++ < 100) {
            bulkRead.txRxPacket();

            bool all_reached = true;
            for (size_t i = 0; i < ids.size(); ++i) {
                int pos = bulkRead.getData(ids[i], ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
                if (std::abs(pos - target_units[i]) > MOVING_STATUS_THRESHOLD) {
                    all_reached = false;
                    break;
                }
            }
            if (all_reached) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        return try_count < 100;
    }

    bool execute(const std::array<Bridge::Step, 5>& steps) {
        for (int i = 0; i < 5; ++i) {
            if (!execute_each(steps[i])) {
                Log::error(std::format("[RobotArmController::execute] Failed to execute step: {}", i));
                return false;
            }
            else {
                Log::debug(std::format("[RobotArmController::execute] Successfully executed step: {}", i));
            }
        }

        return true;
    }
};

#endif //HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP
