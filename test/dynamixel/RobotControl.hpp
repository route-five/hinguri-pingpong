//
// Created by Yeeun on 25. 7. 23.
//

#pragma once
#include "dynamixel_sdk.h"

class RobotControl {
public:
  RobotControl(const char* port_name, int baudrate);
  ~RobotControl();

  bool init();
  void shutdown();

  // 도 단위로 위치 지정
  bool moveArm(double top_deg, double middle_deg, double bottom_deg);

  // 현재 위치에서 상대 이동 → 복귀
  bool moveArmDelta(double d_top, double d_middle, double d_bottom, int delay_ms = 500);

  // 현재 각도 읽기 (도 단위)
  double readAngle(int id);

private:
  const int DXL_ID_TOP = 4;
  const int DXL_ID_MIDDLE = 3;
  const int DXL_ID_BOTTOM = 2;

  const int PROTOCOL_VERSION = 2.0;
  const int ADDR_GOAL_POSITION = 116;
  const int ADDR_TORQUE_ENABLE = 64;
  const int ADDR_PRESENT_POSITION = 132;
  const int TORQUE_ENABLE = 1;
  const int TORQUE_DISABLE = 0;
  const int DXL_MIN_POS = 0;
  const int DXL_MAX_POS = 4095;

  dynamixel::PortHandler* portHandler;
  dynamixel::PacketHandler* packetHandler;

  int degToRaw(double deg);
  double rawToDeg(int raw);
  bool writePosition(int id, int position);
};