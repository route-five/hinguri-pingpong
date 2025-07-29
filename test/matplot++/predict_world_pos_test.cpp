#include <matplot/matplot.h>

#include "../../src/vision/predictor.hpp"

using namespace matplot;

int main() {
    using namespace std;

    // 초기 위치 및 속도
    cv::Point3f init_pos{0.0f, TABLE_HEIGHT, 80.0f}; // 초기 높이 80cm
    cv::Vec3f init_speed{0.0f, -100.0f, -200.0f}; // 평면 이동 + z=0

    vector<double> ys, zs;

    // 시간 간격 (dt가 아님), 단순히 궤적을 따라가기 위한 샘플링 간격
    constexpr float t_step = 0.05f;
    constexpr float total_t = 3.0f;

    int i = 0;
    for (float t = t_step; t <= total_t; i++, t += t_step) {
        auto result = Predictor::predict_world_pos(init_pos, init_speed, t);
        if (!result.has_value()) break;
        const auto& pos = result.value();
        ys.push_back(pos.y);
        zs.push_back(pos.z);
    }

    // 2D 투영 (y-z 평면)
    scatter(ys, zs)->line_width(2).color("blue");
    xlabel("Y (cm)");
    ylabel("Z (cm)");
    title("Predicted Z vs Y trajectory (2D)");

    show();
    return 0;
}
