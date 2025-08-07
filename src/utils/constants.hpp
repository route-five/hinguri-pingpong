//
// Created by 임정훈 on 25. 6. 29.
//

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <opencv2/opencv.hpp>

// Console Color
#define ANSI_BLACK       "\x1B[30m"
#define ANSI_RED         "\x1B[31m"
#define ANSI_GREEN       "\x1B[32m"
#define ANSI_YELLOW      "\x1B[33m"
#define ANSI_BLUE        "\x1B[34m"
#define ANSI_MAGENTA     "\x1B[35m"
#define ANSI_CYAN        "\x1B[36m"
#define ANSI_WHITE       "\x1B[37m"

// 밝은(fg + intensity)
#define ANSI_BRIGHT_BLACK   "\x1B[90m"
#define ANSI_BRIGHT_RED     "\x1B[91m"
#define ANSI_BRIGHT_GREEN   "\x1B[92m"
#define ANSI_BRIGHT_YELLOW  "\x1B[93m"
#define ANSI_BRIGHT_BLUE    "\x1B[94m"
#define ANSI_BRIGHT_MAGENTA "\x1B[95m"
#define ANSI_BRIGHT_CYAN    "\x1B[96m"
#define ANSI_BRIGHT_WHITE   "\x1B[97m"

#define ANSI_RESET       "\x1B[0m"

// Path
const std::string PATH_WORLD_POINTS = "data/points/world.yml";
const std::string PATH_ORBIT = "data/orbit";

// Table Size
constexpr float TABLE_WIDTH = 152.5; // cm
constexpr float TABLE_HEIGHT = 274; // cm
constexpr float TABLE_NET_HEIGHT = 15.25; // cm
constexpr float TABLE_OUTLINE_SIZE = 2.0; // cm
constexpr float TABLE_TAPE_SIZE = 1.8; // cm
constexpr float TABLE_GRID_SIZE = 15.0; // cm

// Predictor Config
constexpr float PREDICT_MAX_TIME = 10.0; // 최대 예측 시간 (초)
constexpr float PREDICT_MIN_TIME = 0.05; // 최소 예측 시간 (초)
constexpr float PREDICT_MAX_ITERATIONS = 20; // 최대 반복 횟수
constexpr float PREDICT_MIN_Y = TABLE_HEIGHT * 1 / 2; // cm
constexpr float PREDICT_MAX_Y = TABLE_HEIGHT; // cm

constexpr int TOP_HISTORY_N = 5; // 탑 가메라 기준 최근 N개의 좌표를 저장함
constexpr int PREDICTOR_HISTORY_N = 10; // 예측 궤적을 저장할 최대 개수

// Environment
constexpr float GRAVITY = 981.0; // 중력 가속도 (cm/s^2)

// Table Constants (cm)
constexpr float TABLE_X_SIZE = 152.5;
constexpr float TABLE_Y_SIZE = 274;
constexpr float TABLE_Z_SIZE = 100;
constexpr float TABLE_BOUNCE_COEFFICIENT = 0.9022811698;

// Orange Ball Colors
const cv::Scalar TOP_ORANGE_MIN(9, 195, 215);
const cv::Scalar TOP_ORANGE_MAX(23, 255, 255);
const cv::Scalar LEFT_ORANGE_MIN(8, 109, 249);
const cv::Scalar LEFT_ORANGE_MAX(32, 249, 255);
const cv::Scalar RIGHT_ORANGE_MIN(8, 118, 232);
const cv::Scalar RIGHT_ORANGE_MAX(23, 233, 255);
const cv::Scalar ORANGE_MIN(9, 195, 215);
const cv::Scalar ORANGE_MAX(23, 255, 255);

// Camera Control
constexpr int CAMERA_WIDTH = 1920;
constexpr int CAMERA_HEIGHT = 1080;

// Camera Matrix
const cv::Mat CAMERA_MATRIX =
    (cv::Mat_<double>(3, 3) << 800, 0, 320, 0, 800, 240, 0, 0, 1);

// Camera Extrinsic Parameters
const cv::Mat CAMERA_ROTATION = cv::Mat::eye(3, 3, CV_64F);
const cv::Mat CAMERA_TRANSLATION = (cv::Mat_<double>(3, 1) << 0, 0, 2);

// Target Plane Height (예: 지면은 0m)
constexpr double TARGET_PLANE_Z = 0.0;

// Falling Test Params
constexpr double TARGET_FRAME_WIDTH = 640;
constexpr double TARGET_FRAME_HEIGHT = 480;
constexpr double TARGET_FPS = 240;
constexpr double CONTOUR_AREA_MIN = 100;

// constexpr double CIRCULARITY_MIN = 0.5;
constexpr double CIRCULARITY_PERFECT = 0.89;
constexpr double CIRCULARITY_THRESHOLD = 0.4;
constexpr double RADIUS_MIN = 1;
constexpr double TOUCHED_MIN = 225;
constexpr double TOUCHED_MAX = 243;
constexpr int TRAILS_SIZE = 30;

constexpr double BASE_AXIS_HEIGHT = 15; // cm
constexpr double AXIS_RADIUS = 12; // cm

// Colors
const cv::Scalar COLOR_RED(0, 0, 255);
const cv::Scalar COLOR_ORANGE(0, 165, 255);
const cv::Scalar COLOR_YELLOW(0, 255, 255);
const cv::Scalar COLOR_GREEN(0, 255, 0);
const cv::Scalar COLOR_CYAN(255, 255, 0);
const cv::Scalar COLOR_BLUE(255, 0, 0);
const cv::Scalar COLOR_MAGENTA(255, 0, 255);

const cv::Scalar COLOR_WHITE(255, 255, 255);
const cv::Scalar COLOR_BLACK(0, 0, 0);

// Dynamixel Actuator
constexpr float CENTER_POS = 2048;
constexpr float MIN_POS_LIMIT = 1024;
constexpr float MAX_POS_LIMIT = 3072;
constexpr float DEGREES_PER_UNIT = 360.0 / 4096.0;

// Dynamixel IDs
// TODO: 시리얼 포트 알아내기
constexpr int MID_SERIAL_PORT = 3; // Mid Camera Serial Port
constexpr int BOT_SERIAL_PORT = 2; // Bot Camera Serial Port

// Linear Motor
constexpr int LINEAR_VEL = 700; // 700
constexpr int LINEAR_ACCEL = 200; // 200
constexpr int LINEAR_DECEL = 200; // 200

constexpr long LINEAR_AXIS_NO = 0;
// constexpr DWORD LINEAR_ABS_REL_MODE = 0; //0->abs, 1->Rel
// constexpr DWORD LINEAR_PROFILE_MODE = 3; //0->symetric trapezode,
// 1->unsymetric trapezode, 2->reserved, 3->symetric S Curve, 4->unsymetric S
// Curve

constexpr long LINEAR_PULSE_PER_10_UNITS = 24390; // heuristic value for cm
constexpr long LINEAR_PULSE_PER_UNIT = 2439;

#endif  // CONSTANTS_HPP
