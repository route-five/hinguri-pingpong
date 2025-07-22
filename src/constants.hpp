//
// Created by 임정훈 on 25. 6. 29.
//

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <opencv2/opencv.hpp>

// Table Size
constexpr float TABLE_WIDTH = 152.5;
constexpr float TABLE_HEIGHT = 274;
constexpr float TABLE_NET_HEIGHT = 15.25;

// Orange Ball Colors
const cv::Scalar ORANGE_MIN(13, 175, 149);  // HSV
const cv::Scalar ORANGE_MAX(25, 255, 255);  // HSV

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
constexpr double RADIUS_MIN = 6;
constexpr double TOUCHED_MIN = 225;
constexpr double TOUCHED_MAX = 243;
constexpr int TRAILS_SIZE = 30;

// Colors
const cv::Scalar COLOR_RED(0, 0, 255);
const cv::Scalar COLOR_GREEN(0, 255, 0);
const cv::Scalar COLOR_BLUE(255, 0, 0);
const cv::Scalar COLOR_YELLOW(0, 255, 255);
const cv::Scalar COLOR_CYAN(255, 255, 0);
const cv::Scalar COLOR_MAGENTA(255, 0, 255);
const cv::Scalar COLOR_WHITE(255, 255, 255);
const cv::Scalar COLOR_BLACK(0, 0, 0);

// Linear Motor
constexpr int LINEAR_VEL = 700;    // 700
constexpr int LINEAR_ACCEL = 200;  // 200
constexpr int LINEAR_DECEL = 200;  // 200

constexpr long LINEAR_AXIS_NO = 0;
// constexpr DWORD LINEAR_ABS_REL_MODE = 0; //0->abs, 1->Rel
// constexpr DWORD LINEAR_PROFILE_MODE = 3; //0->symetric trapezode,
// 1->unsymetric trapezode, 2->reserved, 3->symetric S Curve, 4->unsymetric S
// Curve

constexpr long LINEAR_PULSE_PER_10_UNITS = 2439;  // heuristic value for mm

#endif  // CONSTANTS_HPP
