//
// Created by Yeeun on 25. 7. 25.
//
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

#define RACKET_WIDTH 13.4        // cm
#define RACKET_HEIGHT 14.8       // cm
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

  void close() const {}
};

int dynamixel_set_angle_for(int id, double degree) {
  DynamixelActuator actuator(id);  // id번 모터 사용

  if (!actuator.initialize()) return -1;
  actuator.move_by_degrees(degree);

  actuator.close();
  return 0;
}

void move_linear_for(int x) {
  CLinear_actu actuator;
  printf("Moved linear motor for %d. \n", x);
  actuator.move_actu(x);
}

/*
void control_robot(double top, double mid, double bot, int linear) {
  bot = -bot;
  dynamixel_set_angle_for(TOP_MOTOR_ID, top);
  dynamixel_set_angle_for(MID_MOTOR_ID, mid);
  dynamixel_set_angle_for(BOT_MOTOR_ID, bot);
  move_linear_for(IS_REVERSED ? -linear : linear);
}
*/

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

  double target_x, target_z, target_angle, target_vel, theta;  // rad
  std::cout << "Enter target position (x, z, angle, velocity): ";
  std::cin >> target_x >> target_z >> target_angle >> target_vel;

  double x, t, m, b;

  if (target_x >= 0) {
    theta = asin((target_z - 18.8) / 21.8);  // theta = motor 2 angle
    x = (target_x - (RACKET_HEIGHT / 2 + RACKET_EDGE_RADIUS) * cos(theta)) *
        10;  // cm to mm
  } else {
    theta = asin((target_z - 18.8) / 21.8);
    x = (target_x + (RACKET_HEIGHT / 2 + RACKET_EDGE_RADIUS) * cos(theta)) *
        10;  // cm to mm
  }

  t = rad_to_deg(M_PI / 2 - target_angle);  // top motor angle
  m = rad_to_deg(theta);                    // mid motor angle
  topActuator.move_by_degrees(t);
  midActuator.move_by_degrees(m);
  botActuator.move_by_degrees(-90);
  linearActuator.move_actu(IS_REVERSED ? -x : x);
  botActuator.move_by_degrees(30);

  // while (true) {
  // std::cout << "Move top for: ";
  // std::cin >> t;
  // // Exit condition on top == -1000
  // if (t == -1000) {
  //   topActuator.close();
  //   midActuator.close();
  //   botActuator.close();
  //   break;
  // }
  //
  // std::cout << "Move mid for: ";
  // std::cin >> m;
  //
  // std::cout << "Move bot for: ";
  // std::cin >> b;
  //
  // std::cout << "Move linear for: ";
  // std::cin >> x;

  // Move joints and linear actuator
  // rad to deg
  // }

  // Close the shared port once
  sharedPortHandler->closePort();

  return 0;
}