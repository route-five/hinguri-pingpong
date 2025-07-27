#include "RobotControl.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

RobotControl::RobotControl(const char* port_name, int baudrate) {
    portHandler = dynamixel::PortHandler::getPortHandler(port_name);
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    portHandler->setBaudRate(baudrate);
}

RobotControl::~RobotControl() {
    shutdown();
}

bool RobotControl::init() {
    if (!portHandler->openPort()) {
        std::cerr << "Failed to open port!" << std::endl;
        return false;
    }

    if (!portHandler->setBaudRate(portHandler->getBaudRate())) {
        std::cerr << "Failed to set baudrate!" << std::endl;
        return false;
    }

    for (int id : {DXL_ID_TOP, DXL_ID_MIDDLE, DXL_ID_BOTTOM}) {
        int dxl_result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, TORQUE_ENABLE);
        if (dxl_result != COMM_SUCCESS) {
            std::cerr << "Torque on failed for ID: " << id << std::endl;
            return false;
        }
    }

    return true;
}

void RobotControl::shutdown() {
    for (int id : {DXL_ID_TOP, DXL_ID_MIDDLE, DXL_ID_BOTTOM}) {
        packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, TORQUE_DISABLE);
    }

    portHandler->closePort();
}

bool RobotControl::moveArm(double top_deg, double middle_deg, double bottom_deg) {
    int top_pos = degToRaw(top_deg);
    int middle_pos = degToRaw(middle_deg);
    int bottom_pos = degToRaw(bottom_deg);

    bool result = true;
    result &= writePosition(DXL_ID_TOP, top_pos);
    result &= writePosition(DXL_ID_MIDDLE, middle_pos);
    result &= writePosition(DXL_ID_BOTTOM, bottom_pos);

    return result;
}

bool RobotControl::moveArmDelta(double d_top, double d_middle, double d_bottom, int delay_ms) {
    double curr_top = readAngle(DXL_ID_TOP);
    double curr_middle = readAngle(DXL_ID_MIDDLE);
    double curr_bottom = readAngle(DXL_ID_BOTTOM);

    if (curr_top < 0 || curr_middle < 0 || curr_bottom < 0) {
        std::cerr << "Failed to read current angles." << std::endl;
        return false;
    }

    // Move to offset position
    moveArm(curr_top + d_top, curr_middle + d_middle, curr_bottom + d_bottom);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

    // Return to original position
    moveArm(curr_top, curr_middle, curr_bottom);

    return true;
}

int RobotControl::degToRaw(double deg) {
    int raw = static_cast<int>(deg * 4095.0 / 360.0);
    return std::clamp(raw, DXL_MIN_POS, DXL_MAX_POS);
}

double RobotControl::rawToDeg(int raw) {
    return static_cast<double>(raw) * 360.0 / 4095.0;
}

bool RobotControl::writePosition(int id, int position) {
    int dxl_result = packetHandler->write4ByteTxRx(portHandler, id, ADDR_GOAL_POSITION, position);
    if (dxl_result != COMM_SUCCESS) {
        std::cerr << "Failed to write position to ID: " << id << std::endl;
        return false;
    }
    return true;
}

double RobotControl::readAngle(int id) {
    uint32_t raw_pos = 0;
    uint8_t error = 0;
    int dxl_result = packetHandler->read4ByteTxRx(portHandler, id, ADDR_PRESENT_POSITION, &raw_pos, &error);
    if (dxl_result != COMM_SUCCESS) {
        std::cerr << "Failed to read position from ID: " << id << std::endl;
        return -1.0;
    }
    return rawToDeg(raw_pos);
}