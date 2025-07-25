#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <numbers>
#include <string>

#include "CLinear_actu.h"
#include "constants.hpp"
#include "dynamixel_sdk.h"
#include "sleep.hpp"

#define IS_REVERSED 1

#define ADDR_TORQUE_ENABLE 64
#define ADDR_OPERATING_MODE 11
#define ADDR_GOAL_POSITION 116
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 57600
#define DEVICENAME "COM4"

#define TORQUE_ENABLE 1
#define POSITION_CONTROL_MODE 3

#define CENTER_POS 2048
#define MIN_POS_LIMIT 1024
#define MAX_POS_LIMIT 3072
#define DEGREES_PER_UNIT (360.0 / 4096.0)  // 약 0.088°

#define RACKET_WIDTH 13.4   // cm
#define RACKET_HEIGHT 14.8  // cm
#define RACKET_WIDTH_HALF RACKET_WIDTH / 2
#define RACKET_HEIGHT_HALF RACKET_HEIGHT / 2
#define RACKET_EDGE_RADIUS 14.6  // cm
#define GROUND_EDGE_HEIGHT 12.1  // cm

// Shared handlers for all Dynamixel actuators
static dynamixel::PortHandler* sharedPortHandler =
    dynamixel::PortHandler::getPortHandler(DEVICENAME);
static dynamixel::PacketHandler* sharedPacketHandler =
    dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
static bool sharedPortInitialized = false;

class DynamixelActuator {
 private:
  dynamixel::PortHandler* portHandler;
  dynamixel::PacketHandler* packetHandler;
  int id;

 public:
  explicit DynamixelActuator(const int motor_id) : id{motor_id} {
    // Use shared handlers
    portHandler = sharedPortHandler;
    packetHandler = sharedPacketHandler;
  }

  ~DynamixelActuator() { close(); }

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

    uint8_t dxl_error = 0;

    // Ensure torque is disabled before changing operating mode
    packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);

    int result =
        packetHandler->write1ByteTxRx(portHandler, id, ADDR_OPERATING_MODE,
                                      POSITION_CONTROL_MODE, &dxl_error);
    if (result != COMM_SUCCESS || dxl_error) {
      std::cerr << "Failed to set operating mode" << std::endl;
      return false;
    }

    result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE,
                                           TORQUE_ENABLE, &dxl_error);
    if (result != COMM_SUCCESS || dxl_error) {
      std::cerr << "Failed to enable torque" << std::endl;
      return false;
    }

    std::cout << "Motor initialized successfully" << std::endl;
    return true;
  }

  void move_by_degrees(const double degree_offset) const {
    const int offset_units = static_cast<int>(degree_offset / DEGREES_PER_UNIT);
    const int target_position = CENTER_POS + offset_units;

    // 안전 범위 제한
    if (target_position < MIN_POS_LIMIT || target_position > MAX_POS_LIMIT) {
      std::cerr << "Movement out of range (" << target_position << "). Ignored."
                << std::endl;
      return;
    }

    uint8_t dxl_error = 0;
    int result = packetHandler->write4ByteTxRx(
        portHandler, id, ADDR_GOAL_POSITION, target_position, &dxl_error);
    if (result != COMM_SUCCESS || dxl_error) {
      std::cerr << "Failed to move motor" << std::endl;
    } else {
      std::cout << "Moved " << degree_offset << "position: " << target_position
                << std::endl;
    }
  }

  // Disable torque on this actuator before shutting down
  void close() const {
    uint8_t dxl_error = 0;
    packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, 0, &dxl_error);
  }
};

double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }

int main() {
  // Instantiate actuators and initialize once
  DynamixelActuator topActuator(TOP_MOTOR_ID);
  DynamixelActuator midActuator(MID_MOTOR_ID);
  DynamixelActuator botActuator(BOT_MOTOR_ID);
  CLinear_actu linearActuator;

  // Perform initialization only once
  if (!topActuator.initialize() || !midActuator.initialize() ||
      !botActuator.initialize()) {
    std::cerr << "Failed to initialize one or more motors" << std::endl;
    return -1;
  }
  while (1) {
    double target_x, target_z, target_angle, target_vel, theta;  // rad
    std::cout << "Enter target position (x, z, angle, velocity): ";
    std::cin >> target_x >> target_z >> target_angle >> target_vel;

    double x, t, m, b;
    double r = RACKET_HEIGHT_HALF + RACKET_EDGE_RADIUS;
    double h = GROUND_EDGE_HEIGHT + RACKET_WIDTH_HALF;

    theta = acos((target_z - h) / r);
    x = target_x - r * sin(theta);

    if (target_x == -1000) break;

    topActuator.move_by_degrees(rad_to_deg(-M_PI / 2 + target_angle));
    midActuator.move_by_degrees(rad_to_deg(theta));
    botActuator.move_by_degrees(-90);
    linearActuator.move_actu(IS_REVERSED ? -x : x);
    botActuator.move_by_degrees(30);
  }

  // Close the shared port once
  sharedPortHandler->closePort();

  return 0;
}