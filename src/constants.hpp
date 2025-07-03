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
const cv::Matx33d CAMERA_MATRIX({
    800, 0, 320,
    0, 800, 240,
    0, 0, 1
});

// Camera Extrinsic Parameters
const cv::Mat CAMERA_ROTATION = cv::Mat::eye(3, 3, CV_64F);
const cv::Matx31d CAMERA_TRANSLATION({0, 0, 2});

// Target Plane Height (예: 지면은 0m)
constexpr double TARGET_PLANE_Z = 0.0;


#endif //CONSTANTS_HPP
