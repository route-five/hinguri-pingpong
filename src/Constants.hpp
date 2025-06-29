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
const cv::Scalar ORANGE_MIN = cv::Scalar(33, 120, 200); //BGR
const cv::Scalar ORANGE_MAX = cv::Scalar(90, 180, 280); //BGR

// Camera Control
constexpr int CAMERA_WIDTH = 1920;
constexpr int CAMERA_HEIGHT = 1080;

#endif //CONSTANTS_HPP
