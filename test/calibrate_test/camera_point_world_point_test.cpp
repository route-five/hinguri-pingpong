//
// Created by Hyunseung Ryu on 2025. 7. 29..
//

#include "../../src/utils/draw.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/camera_type.hpp"
#include "../../src/vision/predictor.hpp"

int main() {
    Camera cam(CameraType::LEFT, {0});
    if (!cam.is_opened()) {
        std::cerr << "Failed to open camera: " << cam.get_camera_type().name() << std::endl;
        return -1;
    }

    std::vector<cv::Point2f> camera_points;
    cam.get_camera_type().read_camera_points(camera_points);

    std::vector<cv::Point3f> world_points;
    CameraType::read_world_points(world_points);

    cam.start();

    while (true) {
        auto frame = cam.read();
        if (frame.empty()) continue;

        for (auto& pt : camera_points) {
            cv::circle(frame, pt, 2, COLOR_GREEN, -1, cv::LINE_AA);
        }

        for (auto& pt : world_points) {
            cv::Point2f camera_pos = Predictor::pos_3d_to_2d(cam, pt);
            cv::circle(frame, camera_pos, 2, COLOR_RED, -1, cv::LINE_AA);
        }

        for (float i = 0; i < TABLE_HEIGHT; i += 1.0f) {
            cv::Point2f camera_pos = Predictor::pos_3d_to_2d(cam, cv::Point3f{15, i, 0});
            cv::circle(frame, camera_pos, 2, COLOR_CYAN, -1, cv::LINE_AA);
        }

        Draw::put_text(frame, "Green: Camera Points", {10, 20}, COLOR_GREEN);
        Draw::put_text(frame, "Red: World to Camera Points", {10, 40}, COLOR_RED);

        cv::imshow(std::format("{} camera", cam.get_camera_type().name()), frame);
        if (cv::waitKey(1) == 'q') break;
    }

    cam.stop();
    cv::destroyAllWindows();

    return 0;
}
