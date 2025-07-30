//
// Created by Hyunseung Ryu on 2025. 7. 30..
//

#ifndef DYNAMIXEL_ACTUATOR_HPP
#define DYNAMIXEL_ACTUATOR_HPP

#include "dynamixel_sdk.h"
#include "utils/constants.hpp"

#define ADDR_TORQUE_ENABLE 64
#define ADDR_OPERATING_MODE 11
#define ADDR_GOAL_POSITION 116
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 57600
#define DEVICENAME "COM4"

#define TORQUE_ENABLE 1
#define POSITION_CONTROL_MODE 3

// Shared handlers for all Dynamixel actuators
static dynamixel::PortHandler* sharedPortHandler =
    dynamixel::PortHandler::getPortHandler(DEVICENAME);
static dynamixel::PacketHandler* sharedPacketHandler =
    dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
static bool sharedPortInitialized = false;

class BulkDynamixelActuator {
private:
    dynamixel::PortHandler* portHandler;
    dynamixel::PacketHandler* packetHandler;
    std::vector<int> ids;

public:
    explicit BulkDynamixelActuator(const std::vector<int>& motor_ids) : ids{motor_ids} {
        // Use shared handlers
        portHandler = sharedPortHandler;
        packetHandler = sharedPacketHandler;
    }

    ~BulkDynamixelActuator() {
        for (const auto& id : ids) {
            close(id);
        }
    }

    [[nodiscard]] bool initialize() const {
        if (!sharedPortInitialized) {
            if (!portHandler->openPort()) {
                std::cerr << "Failed to open port" << std::endl;
                return false;
            }
            if (!portHandler->setBaudRate(BAUDRATE)) {
                std::cerr << "Failed to set baudrate" << std::endl;
                return false;
            }
            sharedPortInitialized = true;
        }

        for (const auto& id : ids) {
            uint8_t dxl_error = 0;

            // Ensure torque is disabled before changing operating mode
            packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);

            int result =
                packetHandler->write1ByteTxRx(portHandler, id, ADDR_OPERATING_MODE,
                                              POSITION_CONTROL_MODE, &dxl_error);
            if (result != COMM_SUCCESS || dxl_error) {
                std::cerr << "Failed to set operating mode for motor " << id << std::endl;
                return false;
            }

            result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                                                   TORQUE_ENABLE, &dxl_error);
            if (result != COMM_SUCCESS || dxl_error) {
                std::cerr << "Failed to enable torque for motor " << id << std::endl;
                return false;
            }
        }

        std::cout << "All motors initialized successfully" << std::endl;
        return true;
    }

    bool bulk_move_by_degrees(const std::vector<double>& degree_offsets) const {
        dynamixel::GroupBulkWrite groupBulkWrite(portHandler, packetHandler);
        if (degree_offsets.size() != ids.size()) {
            std::cerr << "Degree offsets size does not match motor IDs size" << std::endl;
            return false;
        }

        std::vector<int> target_positions;
        for (size_t i = 0; i < ids.size(); ++i) {
            int offset_units = static_cast<int>(degree_offsets[i] / DEGREES_PER_UNIT);
            int target_position = CENTER_POS + offset_units;

            // 안전 범위 제한
            if (target_position < MIN_POS_LIMIT || target_position > MAX_POS_LIMIT) {
                std::cerr << "Movement out of range (" << target_position << "). Ignored."
                    << std::endl;
                return true;
            }
            target_positions.push_back(target_position);
        }

        uint8_t dxl_error = 0;
        bool dxl_addparam_result = false;
        for (size_t i = 0; i < ids.size(); ++i) {
            // id, start_address, data_length, *data
            dxl_addparam_result = groupBulkWrite.addParam(
                ids[i], ADDR_GOAL_POSITION, 4,
                reinterpret_cast<uint8_t*>(&target_positions[i])
            );
            if (!dxl_addparam_result) {
                std::cerr << "Failed to add parameter for motor " << ids[i] << std::endl;
                return false;
            }
        }

        bool dxl_comm_result = groupBulkWrite.txPacket();
        if (dxl_comm_result != COMM_SUCCESS) {
            std::cerr << "Failed to write bulk data" << std::endl;
            return false;
        }

        groupBulkWrite.clearParam();

        return true;
    }

    void close(int id) const {
        bulk_move_by_degrees({0, 0, 0});
        uint8_t dxl_error = 0;
        packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);
        std::cout << "Torque disabled for motor " << id << std::endl;
    }
};

#endif //DYNAMIXEL_ACTUATOR_HPP
