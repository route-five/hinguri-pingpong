//
// Created by Hyunseung Ryu on 2025. 7. 24..
//

#include <opencv2/opencv.hpp>

#include "../../../utils/constants.hpp"

/**
 * @brief 탁구대 위의 테이프 교차점의 중앙을 월드 좌표계에서 정의된 3D 점으로 생성합니다.
 * @note constant.hpp에 정의된 상수들을 사용하여 테이프의 크기와 간격을 기반으로 점들을 생성합니다.
 */
int main() {
    std::vector<cv::Point3f> world_pts;

    float x = TABLE_OUTLINE_SIZE + TABLE_GRID_SIZE + TABLE_TAPE_SIZE / 2;
    while (x < TABLE_WIDTH) {
        float y = TABLE_OUTLINE_SIZE + TABLE_GRID_SIZE + TABLE_TAPE_SIZE / 2;

        while (y < TABLE_HEIGHT) {
            world_pts.emplace_back(x, y, TARGET_PLANE_Z);
            y += TABLE_TAPE_SIZE + TABLE_GRID_SIZE;
        }

        x += TABLE_TAPE_SIZE + TABLE_GRID_SIZE;
    }

    std::cout << "World points (cm):" << std::endl;
    for (const auto& pt : world_pts) {
        std::cout << "[" << pt.x << ", " << pt.y << ", " << pt.z << "]" << std::endl;
    }
    std::cout << "Total points: " << world_pts.size() << std::endl;

    cv::FileStorage fs("data/points/world.yml", cv::FileStorage::WRITE);
    fs << "points" << world_pts;
    fs.release();

    return 0;
}
