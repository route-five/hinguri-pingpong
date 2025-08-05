//
// Created by Hyunseung Ryu on 2025. 8. 5..
//

#include "../../src/utils/draw.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/tracker.hpp"

int main() {
    Camera cam(CameraType::TOP, {0, 0}, 120);
    Tracker tracker(ORANGE_MIN, ORANGE_MAX);

    cam.start();

    while (true) {
        cv::Mat frame;
        cam >> frame;
        if (frame.empty()) continue;

        tracker << frame;
        if (const auto find_pos = tracker.get_camera_pos()) {
            Draw::put_circle(frame, find_pos.value().first, find_pos.value().second, COLOR_GREEN);
        }

        cv::imshow("Frame", frame);

        if (cv::waitKey(1) == 'q') {
            std::cout << "Stopping camera capture." << std::endl;
            break;
        }
    }

    cam.stop();
    cv::destroyAllWindows();

    return 0;
}
