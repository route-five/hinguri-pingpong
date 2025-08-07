//
// Created by Hyunseung Ryu on 2025. 7. 22..
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "../calibrator.hpp"
#include "../camera.hpp"
#include "../camera_type.hpp"
#define IMSHOW

int main() {
    Calibrator::Single calibrator(CameraType::RIGHT);

    std::cout << "🔁 finding chessboard corners..." << std::endl;
    std::vector<std::vector<cv::Point2f>> image_points;
    std::vector<std::vector<cv::Point3f>> object_points;
    calibrator.find_chessboard_corners(image_points, object_points, false);
    cv::destroyAllWindows();
    std::cout << "✅ find chessboard corners done." << std::endl << std::endl;

    std::cout << "🔁 calibrating..." << std::endl;
    cv::Rect roi;
    calibrator.calibrate(image_points, object_points, roi);
    std::cout << "✅ calibration done." << std::endl << std::endl;

    std::cout << "🔁 computing camera pose..." << std::endl;
    cv::Mat R, t;
    calibrator.compute_camera_pose(R, t, true);
    std::cout << "✅ camera pose computed." << std::endl << std::endl;

    std::cout << "🔁 computing projection matrix..." << std::endl;
    cv::Mat P;
    calibrator.compute_projection_matrix(R, t, P, true);
    std::cout << "✅ projection matrix computed." << std::endl << std::endl;

    return 0;
}
