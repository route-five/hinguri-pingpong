//
// Created by 임정훈 on 25. 6. 29.
//

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <opencv2/opencv.hpp>

// Table Size (cm)
constexpr float TABLE_X_SIZE = 152.5;
constexpr float TABLE_Y_SIZE = 274;
constexpr float TABLE_Z_SIZE = 100;
constexpr float TABLE_NET_HEIGHT = 15.25;

// Orange Ball Colors
const cv::Scalar ORANGE_MIN = cv::Scalar(12, 140, 160);  // HSV
const cv::Scalar ORANGE_MAX = cv::Scalar(20, 255, 255);  // HSV

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

// Environment
constexpr float GRAVITY = 981;  // 중력 가속도 (cm/s^2)

#endif  // CONSTANTS_HPP
