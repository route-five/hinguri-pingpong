//
// Created by Hyunseung Ryu on 2025. 7. 30..
//

#include "../../src/vision/visualizer.hpp"
#include "../../src/utils/random.hpp"
#include "../../src/vision/bridge.hpp"
#include "../../src/vision/predictor.hpp"

int main() {
    Random<float> random;
    Predictor predictor;
    Visualizer visualizer;

    const float init_x = random.generate(40.0f, TABLE_WIDTH - 40.0f);
    const float init_z = random.generate(10.0f, 50.0f);
    const cv::Point3f init_pos{100.0f, TABLE_HEIGHT, 50.0f};
    const cv::Point3f init_speed{0.0f, -100.0f, -50.0f};

    for (float t = PREDICT_MIN_TIME + 0.01; t <= PREDICT_MAX_TIME - 0.01; t += PREDICT_MIN_TIME) {
        const auto predicted_pos = Predictor::predict_world_pos(init_pos, init_speed, t);
        if (predicted_pos.has_value()) {
            visualizer.add_point(predicted_pos.value());
        }
    }

    const auto t_arrive = Predictor::get_arrive_time(init_pos, init_speed);
    if (!t_arrive.has_value())
        return 1;

    const auto predicted_arrive_pos = Predictor::predict_world_pos(init_pos, init_speed, t_arrive.value());
    if (!predicted_arrive_pos.has_value())
        return 1;

    const auto& predicted_pos = predicted_arrive_pos.value();

    std::vector<double> xs = {init_pos.x, predicted_pos.x};
    std::vector<double> ys = {init_pos.y, predicted_pos.y};
    std::vector<double> zs = {init_pos.z, predicted_pos.z};

    plt::scatter3(xs, ys, zs)
        ->marker_face(true)
        .marker_face_color("g")
        .marker_color("g");

    const auto payload = Bridge::convert(predicted_pos);

    std::vector<double> target_x = {
        payload.x, payload.x + AXIS_RADIUS * std::cos(payload.theta * std::numbers::pi_v<float> / 180.0f)
    };
    std::vector<double> target_y = {0, 0};
    std::vector<double> target_z = {
        0, BASE_AXIS_HEIGHT + AXIS_RADIUS * std::sin(payload.theta * std::numbers::pi_v<float> / 180.0f)
    };

    plt::scatter3(target_x, target_y, target_z)
        ->marker_face(true)
        .marker_face_color("r")
        .marker_color("r");

    plt::plot3(target_x, target_y, target_z)
        ->color("r");

    visualizer.show();

    return 0;
}