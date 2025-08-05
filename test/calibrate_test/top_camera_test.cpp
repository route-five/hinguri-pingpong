//
// Created by Hyunseung Ryu on 2025. 7. 29..
//

#include "utils/draw.hpp"
#include "vision/camera.hpp"
#include "vision/camera_type.hpp"
#include "vision/predictor.hpp"
#include "vision/tracker.hpp"

int main() {
    Camera cam(CameraType::TOP, { 0, cv::CAP_MSMF }, 120);
    if (!cam.is_opened()) {
        std::cerr << "Failed to open camera: " << cam.get_camera_type().name() << std::endl;
        return -1;
    }

    Predictor predictor;
    Tracker tracker{ ORANGE_MIN, ORANGE_MAX };

    std::vector<cv::Point2f> camera_points;
    std::vector<cv::Point3f> world_points;

    cv::FileStorage fs_top(cam.get_camera_type().camera_points_path(), cv::FileStorage::READ);
    fs_top["points"] >> camera_points;
    fs_top.release();

    cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
    fs_world["points"] >> world_points;
    fs_world.release();

    cam.start();

    while (true) {
        auto frame = cam.read();
        if (frame.empty()) continue;

        for (auto& pt : camera_points) {
            cv::circle(frame, pt, 3, COLOR_GREEN, -1, cv::LINE_AA);
        }

        for (auto& pt : world_points) {
            cv::Point2f camera_pos = predictor.pos_3d_to_2d(cam, pt);
            cv::circle(frame, camera_pos, 3, COLOR_BLUE, -1, cv::LINE_AA);
        }

        // tracker.update(frame);
        // const auto ret = tracker.get_camera_pos();
        // if (ret.has_value()) {
        //     const auto [center, radius] = ret.value();
        //     cv::circle(frame, center, radius, COLOR_GREEN, -1, cv::LINE_AA);
        //     Draw::put_text(
        //         frame,
        //         Draw::to_string("Tracking Camera Pos", center, "px"),
        //         {10, 30}
        //     );
        //
        //     const auto world_pos = predictor.birds_eye_view(center);
        //     Draw::put_text(
        //         frame,
        //         Draw::to_string("World Pos", world_pos, "cm"),
        //         {10, 70}
        //     );
        //
        //     const auto camera_pos = predictor.pos_3d_to_2d(world_pos, CameraType::TOP);
        //     cv::circle(frame, camera_pos, 5, COLOR_RED, -1, cv::LINE_AA);
        //     Draw::put_text(
        //         frame,
        //         Draw::to_string("Predicted Camera Pos", camera_pos, "px"),
        //         {10, 110}
        //     );
        // }

        cv::imshow("Top Camera", frame);
        if (cv::waitKey(1) == 'q') break;
    }

    cam.stop();
    cv::destroyAllWindows();

    return 0;
}
