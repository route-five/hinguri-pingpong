//
// Created by Hyunseung Ryu on 2025. 7. 29..
//

#include "../../src/utils/draw.hpp"
#include "../../src/vision/camera.hpp"
#include "../../src/vision/camera_type.hpp"
#include "../../src/vision/predictor.hpp"

int main() {
    Camera cam(CameraType::TOP, {0});
    if (!cam.is_opened()) {
        std::cerr << "Failed to open camera: " << cam.get_camera_type().get_name() << std::endl;
        return -1;
    }

    std::vector<cv::Point2f> camera_points;
    cam.get_camera_type().read_camera_points(camera_points);

    std::vector<cv::Point3f> world_points;
    CameraType::read_world_points(world_points);

    std::vector<std::pair<cv::Point2f, cv::Scalar>> projected_points;
    constexpr float max_z = 80.0f;

    for (const auto& pt : world_points) {
        for (float z = 0.f; z <= max_z; z += 5.f) {
            const auto new_pt = cv::Point3f{pt.x, pt.y, z};
            const float normalized_z = z / max_z;
            const int colormap_index = static_cast<int>(normalized_z * 255);

            cv::Mat color_mat(1, 1, CV_8UC1, cv::Scalar(colormap_index));
            cv::Mat rgb_mat;
            cv::applyColorMap(color_mat, rgb_mat, cv::ColormapTypes::COLORMAP_RAINBOW);
            const cv::Vec3b color = rgb_mat.at<cv::Vec3b>(0, 0);

            projected_points.emplace_back(Predictor::pos_3d_to_2d(cam, new_pt), color);
        }
    }

    cam.start();

    while (true) {
        auto frame = cam.read();
        if (frame.empty()) continue;

        for (auto& pt : camera_points) {
            cv::circle(frame, pt, 2, COLOR_WHITE, -1, cv::LINE_AA);
        }

        for (const auto& [pt, color] : projected_points) {
            cv::circle(frame, pt, 2, color, -1, cv::LINE_AA);
        }

        Draw::put_text_border(frame, "Camera Points", {10, 20}, COLOR_WHITE);
        Draw::put_text_border(frame, "World to Camera Points", {10, 40}, COLOR_RED);

        cv::imshow(std::format("{} camera", cam.get_camera_type().get_name()), frame);
        if (cv::waitKey(1) == 'q') break;
    }

    cam.stop();
    cv::destroyAllWindows();

    return 0;
}
