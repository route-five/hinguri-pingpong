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
    const cv::Point3f init_pos{10.0f, TABLE_HEIGHT, 50.0f};
    const cv::Point3f init_speed{20.0f, -100.0f, -100.0f};

    for (float t = PREDICT_MIN_TIME + 0.01; t <= PREDICT_MAX_TIME - 0.01; t += 0.01) {
        const auto predicted_pos = Predictor::predict(init_pos, init_speed, t);
        if (predicted_pos.has_value()) {
            visualizer.add_point(predicted_pos.value().predicted_position);
        }
    }

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
