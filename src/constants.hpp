//
// Created by 임정훈 on 25. 6. 29.
//

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <opencv2/opencv.hpp>

// Table Size
constexpr float TABLE_WIDTH = 152.5;
constexpr float TABLE_HEIGHT = 274;

// Orange Ball Colors
const cv::Scalar ORANGE_MIN = cv::Scalar(12, 140, 160); // HSV
const cv::Scalar ORANGE_MAX = cv::Scalar(20, 255, 255); // HSV

// Camera Control
constexpr int CAMERA_WIDTH = 1920;
constexpr int CAMERA_HEIGHT = 1080;

// Camera Matrix
const cv::Mat CAMERA_MATRIX = (cv::Mat_<double>(3, 3) <<
    800, 0, 320,
    0, 800, 240,
    0, 0, 1
);

// Camera Extrinsic Parameters
const cv::Mat CAMERA_ROTATION = cv::Mat::eye(3, 3, CV_64F);
const cv::Mat CAMERA_TRANSLATION = (cv::Mat_<double>(3, 1) << 0, 0, 2);

// Target Plane Height (예: 지면은 0m)
constexpr double TARGET_PLANE_Z = 0.0;

// Falling Test Params
constexpr double TARGET_FRAME_WIDTH = 640;
constexpr double TARGET_FRAME_HEIGHT = 480;
constexpr double TARGET_FPS = 120;
constexpr double CONTOUR_AREA_MIN = 100;
constexpr double CIRCULARITY_MIN = 0.5;
constexpr double RADIUS_MIN = 7;
constexpr double TOUCHED_MIN = 225;
constexpr double TOUCHED_MAX = 243;
constexpr int TRAILS_SIZE = 30;

// Linear Motor
constexpr int LINEAR_MOTOR_VEL = 700;
constexpr int LINEAR_MOTOR_ACCEL = 200;

#endif //CONSTANTS_HPP
