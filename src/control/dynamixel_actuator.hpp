#ifndef DYNAMIXEL_ACTUATOR_HPP
#define DYNAMIXEL_ACTUATOR_HPP

#include <vector>
#include <cmath>
#include <iostream>
#include "dynamixel_sdk.h"
#include "../utils/log.hpp"
#include "utils/constants.hpp"
#include "vision/bridge.hpp"

// Control table addresses
#define ADDR_TORQUE_ENABLE    64
#define ADDR_OPERATING_MODE   11
#define ADDR_GOAL_POSITION    116
#define PROTOCOL_VERSION      2.0
#define BAUDRATE              57600
#define DEVICENAME            "COM7"

#define TORQUE_ENABLE         1
#define POSITION_CONTROL_MODE 3

// For monitoring when each motor reaches its goal
#define ADDR_PRESENT_POSITION      132
#define LEN_PRESENT_POSITION       4
#define MOVING_STATUS_THRESHOLD    20  // units

// Shared handlers
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
    explicit BulkDynamixelActuator(const std::vector<int>& motor_ids)
        : portHandler(sharedPortHandler),
          packetHandler(sharedPacketHandler),
          ids(motor_ids) {
    }

    ~BulkDynamixelActuator() {
        for (const int id : ids) close(id);
    }

    [[nodiscard]] bool initialize() const {
        if (!sharedPortInitialized) {
            if (!portHandler->openPort()) {
                Log::error("[Dynamixel::initialize] Failed to open port: " + std::string(DEVICENAME));
                return false;
            }
            if (!portHandler->setBaudRate(BAUDRATE)) {
                Log::error("[Dynamixel::initialize] Failed to set baudrate: " + std::to_string(BAUDRATE));
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
                Log::error(std::format("[Dynamixel::initialize] Failed to set operating mode for motor {}, error code: {}", id, err));
                return false;
            }
            res = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                                                TORQUE_ENABLE, &err);
            if (res != COMM_SUCCESS || err) {
                Log::error(std::format("[Dynamixel::initialize] Failed to enable torque for motor {}, error code: {}", id, err));
                return false;
            }
        }

        Log::info("[Dynamixel::initialize] All motors initialized successfully");
        return true;
    }

    bool bulk_move_by_degrees(const std::vector<float>& degrees) {
        Log::debug("[Dynamixel::bulk_move_by_degrees] bulk_move_by_degrees called.");
        dynamixel::GroupBulkWrite groupBulkWrite(portHandler, packetHandler);

        if (degrees.size() != ids.size()) {
            Log::warn(std::format("[Dynamixel::bulk_move_by_degrees] Degree offsets size ({}) does not match motor IDs size ({}).", degrees.size(), ids.size()));
            return false;
        }

        std::vector<int> target_positions;
        for (size_t i = 0; i < ids.size(); ++i) {
            const int unit = Bridge::to_dynamixel_unit_from_deg(degrees[i]);
            Log::debug(std::format("[Dynamixel::bulk_move_by_degrees] Target position for motor {}: {} deg ({} unit)", ids[i], degrees[i], unit));

            // 안전 범위 제한
            /*if (target_position < MIN_POS_LIMIT || target_position > MAX_POS_LIMIT) {
                std::cerr << "Movement out of range (idx=" << i << ", value=" << target_position << "). Clamped." << std::endl;
                target_position = std::clamp(target_position, MIN_POS_LIMIT, MAX_POS_LIMIT);
            }*/

            target_positions.push_back(unit);
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
                Log::error(std::format("[Dynamixel::bulk_move_by_degrees] Failed to add parameter for motor {}.", ids[i]));
                return false;
            }
        }

        bool dxl_comm_result = groupBulkWrite.txPacket();
        if (dxl_comm_result != COMM_SUCCESS) {
            Log::warn("[Dynamixel::bulk_move_by_degrees] Failed to write bulk data, attempting restart.");
            if (!restart()) {
                Log::error("[Dynamixel::bulk_move_by_degrees] Restart failed!");
                return false;
            }
        }

        groupBulkWrite.clearParam();

        return true;
    }

    // New: move to 'degrees' then poll PRESENT_POSITION until within threshold,
    //      set *done_flag = true when reached.
    bool move_and_wait_by_degrees(const std::vector<float>& degrees, bool* done_flag) {
        *done_flag = false;

        Log::debug("[Dynamixel::move_and_wait_by_degrees] move_and_wait_by_degrees called.");

        if (degrees.size() != ids.size()) {
            Log::warn(std::format("[Dynamixel::move_and_wait_by_degrees] Degree offsets size ({}) does not match motor IDs size ({}).", degrees.size(), ids.size()));
            return false;
        }

        // 1) Convert to dynamixel units
        std::vector<int> target_units;
        for (size_t i = 0; i < ids.size(); ++i) {
            const int unit = Bridge::to_dynamixel_unit_from_deg(degrees[i]);
            Log::debug(std::format("[Dynamixel::move_and_wait_by_degrees] Target position for motor {}: {} deg ({} unit)", ids[i], degrees[i], unit));

            target_units.push_back(unit);
        }

        // 2) Bulk-write goal positions
        dynamixel::GroupBulkWrite writer(portHandler, packetHandler);
        for (size_t i = 0; i < ids.size(); ++i) {
            if (!writer.addParam(ids[i], ADDR_GOAL_POSITION, 4, reinterpret_cast<uint8_t*>(&target_units[i]))) {
                Log::error(std::format("[Dynamixel::move_and_wait_by_degrees] Failed to add parameter for motor {}.", ids[i]));
                return false;
            }
        }

        if (writer.txPacket() != COMM_SUCCESS) {
            Log::warn("[Dynamixel::move_and_wait_by_degrees] Failed to write bulk data, attempting restart.");
            if (!restart()) {
                Log::error("[Dynamixel::move_and_wait_by_degrees] Restart failed!");
                return false;
            }
        }
        writer.clearParam();

        // 3) Setup bulk-read for PRESENT_POSITION
        dynamixel::GroupBulkRead reader(portHandler, packetHandler);
        for (int id : ids) {
            if (!reader.addParam(id, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION)) {
                Log::error(std::format("[Dynamixel::move_and_wait_by_degrees] Failed to add parameter for PRESENT_POSITION for motor {}.", id));
                return false;
            }
        }

        // 4) Poll until all motors are within threshold
        int try_count = 0;

        while (try_count++ < 5) {
            int result = reader.txRxPacket();
            if (result != COMM_SUCCESS) {
                Log::warn("[Dynamixel::move_and_wait_by_degrees] Failed to read bulk data, attempting restart.");
                if (!restart()) {
                    Log::error("[Dynamixel::move_and_wait_by_degrees] Restart failed!");
                    return false;
                }
            }
            bool all_reached = true;
            for (size_t i = 0; i < ids.size(); ++i) {
                int pos = reader.getData(ids[i], ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
                if (std::abs(pos - target_units[i]) > MOVING_STATUS_THRESHOLD) {
                    all_reached = false;
                    break;
                }
            }
            if (all_reached) break;
        }

        if (try_count == 5) {
            Log::warn("[Dynamixel::move_and_wait_by_degrees] Failed to read COMM_SUCCESS from packet (tried 10 times).");
            return false;
        }

        *done_flag = true;
        return true;
    }

    bool restart() {
        sharedPortHandler->closePort();
        sharedPortInitialized = false;
        return initialize();

        /*uint8_t dxl_error = 0;
        int dxl_comm_result = COMM_TX_FAIL;

        for (size_t i = 0; i < ids.size(); ++i) {
            dxl_comm_result = sharedPacketHandler->reboot(sharedPortHandler, ids[i], &dxl_error);

            if (dxl_comm_result != COMM_SUCCESS) {
                std::cerr << sharedPacketHandler->getTxRxResult(dxl_comm_result) << std::endl;
                return 0;
            }
            else if (dxl_error != 0) {
                std::cerr << sharedPacketHandler->getRxPacketError(dxl_error) << std::endl;
                return 0;
            }
        }
        return 1;*/
    }

    void close(int id) {
        uint8_t err = 0;
        packetHandler->write1ByteTxRx(portHandler, id,ADDR_TORQUE_ENABLE, 0, &err);
        if (sharedPortInitialized) {
            sharedPortHandler->closePort();
            sharedPortInitialized = false;
        }

        Log::info(std::format("[Dynamixel::close] Torque disabled for motor {}.", id));
    }
};

#endif // DYNAMIXEL_ACTUATOR_HPP
