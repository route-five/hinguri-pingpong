#include "dynamixel_sdk.h"
#include <iostream>
#include <thread>
#include <chrono>


const char* PORT_NAME = "/dev/ttyUSB0"; // 알단 이건 내 마음대로 한 것임
const int BAUDRATE = 1000000;
const int PROTOCOL_VERSION = 1; // MX-28T, MX-64T both use Protocol 1.0

const int ID_MOTOR_SWING = 1; // MX-28T (스윙)
const int ID_MOTOR_DIRECTION = 2; // MX-64T (좌우)

const int ADDR_GOAL_POSITION = 30;
const int ADDR_PRESENT_POSITION = 36;

const int ADDR_TORQUE_ENABLE = 24;
const int TORQUE_ENABLE = 1;
const int TORQUE_DISABLE = 0;

const int MOVING_SPEED = 100; // 이거 걍 놔둔건데 바꿔야할 듯?

const int POSITION_LEFT = 300;   // 좌측 위치
const int POSITION_RIGHT = 700;  // 우측 위치
const int POSITION_CENTER = 512; // 중간 위치 (기본값)

const int SWING_BACK = 600; // 백스윙 위치
const int SWING_HIT = 300;  // 스윙 포지션

void moveMotor(dynamixel::PortHandler* portHandler, dynamixel::PacketHandler* packetHandler, int id, int position) {
    int dxl_comm_result;
    uint8_t dxl_error;
    uint16_t pos = static_cast<uint16_t>(position);
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, ADDR_GOAL_POSITION, pos, &dxl_error);

    if (dxl_comm_result != COMM_SUCCESS) {
        std::cerr << "Failed to move motor " << id << ": " << packetHandler->getTxRxResult(dxl_comm_result) << std::endl;
    } else if (dxl_error != 0) {
        std::cerr << "Dynamixel error " << static_cast<int>(dxl_error) << std::endl;
    }
}

void enableTorque(dynamixel::PortHandler* portHandler, dynamixel::PacketHandler* packetHandler, int id) {
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);

    if (dxl_comm_result != COMM_SUCCESS) {
        std::cerr << "Failed to enable torque on motor " << id << std::endl;
    }
}

// 왕복 왔다갔다 하는거임
void moveLeftRightLoop(dynamixel::PortHandler* portHandler, dynamixel::PacketHandler* packetHandler, int id) {
    while (true) {
        moveMotor(portHandler, packetHandler, id, POSITION_LEFT);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        moveMotor(portHandler, packetHandler, id, POSITION_RIGHT);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    }
// 휘두르고 복귀하는거임
void swingOnce(dynamixel::PortHandler* portHandler, dynamixel::PacketHandler* packetHandler, int id) {
    moveMotor(portHandler, packetHandler, id, SWING_BACK);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    moveMotor(portHandler, packetHandler, id, SWING_HIT);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    moveMotor(portHandler, packetHandler, id, SWING_BACK);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int main() {
    auto* portHandler = dynamixel::PortHandler::getPortHandler(PORT_NAME);
    auto* packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

    if (!portHandler->openPort()) {
        std::cerr << "Failed to open port" << std::endl;
        return -1;
    }
    if (!portHandler->setBaudRate(BAUDRATE)) {
        std::cerr << "Failed to set baudrate" << std::endl;
        return -1;
    }

    enableTorque(portHandler, packetHandler, ID_MOTOR_SWING);
    enableTorque(portHandler, packetHandler, ID_MOTOR_DIRECTION);

    // 좌우 왔다갔다 + 중간에 휘두르기 (test 용임)
    std::thread leftRightThread([&]() {
        moveLeftRightLoop(portHandler, packetHandler, ID_MOTOR_DIRECTION);
    });

    while (true) {
        swingOnce(portHandler, packetHandler, ID_MOTOR_SWING);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    leftRightThread.join();

    return 0;
}