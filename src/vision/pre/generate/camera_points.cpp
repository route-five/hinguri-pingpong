//
// Created by Hyunseung Ryu on 2025. 7. 24..
//

#include <opencv2/opencv.hpp>

#include "../../calibrator.hpp"
#include "../../camera.hpp"
#include "../../camera_type.hpp"

/**
 * @brief 카메라 좌표계에서 탁구대 위의 테이프 교차점의 중앙을 클릭하여 좌표를 수집합니다.
 * @note:
 *  1. 주어진 방향에 맞게 탁구대 위의 테이프의 교차점의 중앙을 클릭해야 합니다.
 *  2. 하얀 테두리 있는 가장자리 라인은 전부 무시하고 그 안쪽 절연 테이프의 교차점만 클릭해야 합니다.
 *  3. 문 쪽에서 보든 창문 쪽에서 보든 클릭 방향과 위치는 항상 동일합니다.
 *
 * <code>
 *                   Door
 *      ┌──────────────┬──────────────┐
 *      │ 1. ----------│------------> │
 *      │ 2. ----------│------------> │
 * Bot  ├──────────────┼──────────────┤
 *      │ 3. ----------│------------> │
 *      │ 4. ----------│------------> │
 *      └──────────────┴──────────────┘
 * </code>
 */
int main() {
    const CameraType camera_type = CameraType::LEFT;
    std::vector<cv::Point2f> camera_points;

    Calibrator calibrator(camera_type);

    Camera cap({}, {1280, 720}, 120);
    if (!cap.is_opened()) {
        std::cerr << "Failed to open camera" << std::endl;
        return -1;
    }

    cap.start();

    cv::Mat frame;
    bool frozen = false;
    cv::Mat frozen_frame;

    const std::string window_name = "Click Points - Press SPACE to freeze, Q to quit";
    cv::namedWindow(window_name);

    cv::setMouseCallback(window_name, [](const int event, int x, int y, int, void* userdata) {
        if (event == cv::EVENT_LBUTTONDOWN) {
            auto* points = static_cast<std::vector<cv::Point2f>*>(userdata);
            points->emplace_back(x, y);
            std::cout << "Clicked: [" << x << ", " << y << "]" << std::endl;
        }
    }, &camera_points);

    while (true) {
        if (!frozen) {
            frame = cap.read();
            if (frame.empty()) break;
            cv::imshow(window_name, frame);
        }
        else {
            cv::imshow(window_name, frozen_frame);
        }

        const int key = cv::waitKey(30);
        if (key == ' ') {
            frozen = !frozen;
            if (frozen) frozen_frame = frame.clone();
        }
        else if (key == 'q' || key == 'Q') {
            std::cout << "Total points: " << camera_points.size() << std::endl;
            break;
        }
    }

    cap.stop();
    cv::destroyAllWindows();

    if (!camera_points.empty()) {
        cv::FileStorage fs(camera_type.camera_points_path(), cv::FileStorage::WRITE);
        fs << "points" << camera_points;
        fs.release();
    }

    return 0;
}
