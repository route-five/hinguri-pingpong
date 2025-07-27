//
// Created by Hyunseung Ryu on 2025. 7. 27..
//

#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>

#include "../../src/utils/constants.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/tracker.hpp"

int main() {
    Camera cam({0, 0}, {1280, 720}, 120); // 웹캠 또는 비디오 파일

    if (!cam.is_opened()) {
        std::cerr << "Cannot open camera/video\n";
        return -1;
    }

    // 배경 제거기 초기화 (MOG2 or KNN)
    cam.start();

    while (true) {
        cv::Mat frame = cam.read();
        if (frame.empty()) break;

        Tracker tracker{frame, ORANGE_MIN, ORANGE_MAX};

        cv::imshow("moving_color_mask", tracker.get_moving_color_mask());

        if (cv::waitKey(30) == 'q') break;
    }

    cam.stop();

    return 0;
}
