//
// Created by Hyunseung Ryu on 2025. 7. 27..
//

#include "../../src/vision/camera.hpp"

int main() {
    Camera cam({0, 0}, {1280, 720}, 120);
    if (!cam.is_opened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    cam.start();

    int cnt = 0;

    while (true) {
        cv::Mat frame = cam.read();
        if (!frame.empty()) continue;

        cv::imshow("Frame", frame);

        const int key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == ' ') {
            cv::imwrite(std::format("img/balls/{}.png", cnt++), frame);
        }
    }

    cam.stop();

    return 0;
}
