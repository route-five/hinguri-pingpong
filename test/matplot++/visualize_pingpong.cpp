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

    constexpr float pi = std::numbers::pi_v<float>;

    constexpr float init_vel = 630.0f; // 305 cm/s
    constexpr float init_launch_angle = 0.0f; // 0 degrees in radians
    constexpr float init_min_direction_angle = pi / 2 - 0.25732; // 180 - 14.7433 degrees in radians
    constexpr float init_max_direction_angle = pi / 2 + 0.23256; // 180 + 13.3247 degrees in radians
    constexpr float init_y = 225.0f; // 225 cm

    const cv::Point3f init_pos{TABLE_WIDTH / 2, init_y, 40.0f};
    const float init_direction_angle = init_max_direction_angle; // random.generate(init_min_direction_angle, init_max_direction_angle);

    const cv::Point3f init_speed{
        -init_vel * std::cos(init_launch_angle) * std::cos(init_direction_angle),
        -init_vel * std::cos(init_launch_angle) * std::sin(init_direction_angle),
        init_vel * std::sin(init_launch_angle)
    };

    std::cout << "init_pos: " << init_pos << ", init_speed: " << init_speed << ", init_velocity: " << cv::norm(init_speed) << std::endl;

    for (float t = PREDICT_MIN_TIME + 0.01; t <= PREDICT_MAX_TIME - 0.01; t += 0.01) {
        const auto predicted_pos = Predictor::predict(init_pos, init_speed, t);
        if (predicted_pos.has_value()) {
            visualizer.add_point(predicted_pos.value().predicted_position);
            if (std::abs(predicted_pos.value().predicted_position.y) < 5.0f) {
                std::cout << "last t: " << t << ", predicted_pos: " << predicted_pos.value().predicted_position << std::endl;
            }
        }
    }

    const auto t_arrive = Predictor::get_arrive_time(init_pos, init_speed);
    if (!t_arrive.has_value()) {
        std::cout << "No arrive time found." << std::endl;
        return 1;
    }

    const auto arrive = Predictor::predict(init_pos, init_speed, t_arrive.value());
    if (!arrive.has_value()) {
        std::cout << "No arrive position found." << std::endl;
        return 1;
    }

    std::cout << "arrive time: " << t_arrive.value() << ", arrive pos: " << arrive.value().predicted_position << std::endl;

    // const auto t_arrive = Predictor::get_arrive_time(init_pos, init_speed);
    // if (!t_arrive.has_value())
    //     return 1;
    //
    // const auto predicted_arrive_pos = Predictor::predict(init_pos, init_speed, t_arrive.value());
    // if (!predicted_arrive_pos.has_value())
    //     return 1;
    //
    // const auto& predict = predicted_arrive_pos.value();
    //
    // std::vector<double> xs = {init_pos.x, predict.predicted_position.x};
    // std::vector<double> ys = {init_pos.y, predict.predicted_position.y};
    // std::vector<double> zs = {init_pos.z, predict.predicted_position.z};
    //
    // plt::scatter3(xs, ys, zs)
    //     ->marker_face(true)
    //     .marker_face_color("g")
    //     .marker_color("g");

    visualizer.show();

    return 0;
}
