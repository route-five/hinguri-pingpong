// Created by Hyunseung Ryu on 2025. 8. 8..
// Modified: profile tuning + threshold 기반 스텝 전환

#ifndef HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP
#define HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP

#include <thread>
#include <vector>
#include <sstream>
#include "dynamixel_sdk.h"
#include "../utils/constants.hpp"
#include "../utils/log.hpp"
#include "../vision/bridge.hpp"
#include "dynamixel_actuator.hpp"

constexpr uint16_t LEN_GOAL_POSITION = 4;   // Goal Position 길이 (4바이트)
constexpr uint8_t  ADDR_PRESENT_MOVING = 122; // Moving 플래그 주소
constexpr uint16_t LEN_PRESENT_MOVING = 1;   // Moving 플래그 길이

// 프로필 튜닝 레지스터 (X 시리즈 예시 주소)
// 자신의 모델에 맞게 조정하세요
constexpr uint16_t ADDR_PROFILE_ACCEL = 108; // Profile Acceleration
constexpr uint16_t ADDR_PROFILE_VEL = 112; // Profile Velocity
constexpr uint32_t PROFILE_ACCEL_VALUE = 200; // 100 가속도
constexpr uint32_t PROFILE_VEL_VALUE = 800; // 750 최고속도

class RobotArmController {
private:
    std::array<int, 4>         ids = DYNAMIXEL_IDS;
    dynamixel::PortHandler* portHandler = sharedPortHandler;
    dynamixel::PacketHandler* packetHandler = sharedPacketHandler;

    // MOVING flag 재사용 reader
    mutable dynamixel::GroupSyncRead syncRead;

public:
    RobotArmController()
        : syncRead(portHandler, packetHandler, ADDR_PRESENT_MOVING, LEN_PRESENT_MOVING)
    {
        for (auto id : ids)
            syncRead.addParam(static_cast<uint8_t>(id));
    }

    [[nodiscard]] bool initialize() const {
        if (!sharedPortInitialized) {
            if (!portHandler->openPort() ||
                !portHandler->setBaudRate(BAUDRATE)) {
                Log::error("[initialize] Port open/baudrate failed");
                return false;
            }
            sharedPortInitialized = true;
        }
        for (int id : ids) {
            uint8_t err = 0;
            // position mode
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &err);
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_OPERATING_MODE,
                POSITION_CONTROL_MODE, &err);
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                TORQUE_ENABLE, &err);
            // profile tuning
            packetHandler->write4ByteTxRx(portHandler, id, ADDR_PROFILE_ACCEL,
                PROFILE_ACCEL_VALUE, &err);
            packetHandler->write4ByteTxRx(portHandler, id, ADDR_PROFILE_VEL,
                PROFILE_VEL_VALUE, &err);
        }
        Log::info("[initialize] Motor profiles configured");
        return true;
    }

    bool execute_each(const Bridge::Step& step) {
        // lazy init
        if (!sharedPortInitialized && !initialize()) return false;

        // 각도→unit
        std::vector<int> target = {
            Bridge::to_dynamixel_unit_from_deg(step.bot_angle),
            Bridge::to_dynamixel_unit_from_deg(step.mid_angle),
            Bridge::to_dynamixel_unit_from_deg(step.top_angle),
            Bridge::to_dynamixel_unit_from_deg(step.upper_angle)
        };

        // 1) SyncWrite → 즉시 실행
        dynamixel::GroupSyncWrite syncWrite(
            portHandler, packetHandler,
            ADDR_GOAL_POSITION, LEN_GOAL_POSITION
        );
        for (size_t i = 0; i < ids.size(); ++i) {
            int u = target[i];
            uint8_t p[4] = {
                DXL_LOBYTE(DXL_LOWORD(u)),
                DXL_HIBYTE(DXL_LOWORD(u)),
                DXL_LOBYTE(DXL_HIWORD(u)),
                DXL_HIBYTE(DXL_HIWORD(u))
            };
            syncWrite.addParam(static_cast<uint8_t>(ids[i]), p);
        }
        if (syncWrite.txPacket() != COMM_SUCCESS) {
            Log::error("[execute_each] SyncWrite failed");
            return false;
        }
        syncWrite.clearParam();

        // 2) MOVING flag 폴링 (빠르게)
        int polls = 0;
        while (polls++ < 200) {
            syncRead.txRxPacket();
            bool moving = false;
            for (auto id : ids) {
                if (syncRead.getData(
                    static_cast<uint8_t>(id),
                    ADDR_PRESENT_MOVING,
                    LEN_PRESENT_MOVING
                ) != 0) {
                    moving = true;
                    break;
                }
            }
            if (!moving) break;
            std::this_thread::yield();
        }
        if (polls >= 200) {
            Log::error("[execute_each] timeout");
            return false;
        }

        return true;
    }

    bool execute(const std::array<Bridge::Step, STEP_LENGTH>& steps) {
        for (auto& s : steps) {
            if (!execute_each(s)) return false;
        }
        return true;
    }
};

#endif // HINGURI_PINGPONG_ROBOT_ARM_CONTROLLER_HPP
