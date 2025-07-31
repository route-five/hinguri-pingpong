//
// Created by Hyunseung Ryu on 2025. 8. 1..
//

#include <opencv2/opencv.hpp>

#include "../camera_type.hpp"

int main() {
    std::vector<cv::Point2f> camera_points;

    cv::FileStorage from("data/points/top_camera_og.yml", cv::FileStorage::READ);
    from["points"] >> camera_points;
    from.release();

    for (auto& pt : camera_points) {
        pt.x *= 0.5f;
        pt.y *= 0.5f;
    }

    cv::FileStorage to(CameraType::TOP.camera_points_path(), cv::FileStorage::WRITE);
    to << "points" << camera_points;
    to.release();

    return 0;
}
