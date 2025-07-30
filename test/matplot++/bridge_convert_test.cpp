//
// Created by Hyunseung Ryu on 2025. 7. 30..
//

#include <matplot/matplot.h>
#include <random>
#include <numbers>
#include <cmath>
#include "../../src/utils/constants.hpp"
#include "../../src/vision/bridge.hpp"

using namespace matplot;

int main() {
    // 랜덤 도착 위치 생성 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    constexpr float X_MIN = 0.0f;
    constexpr float X_MAX = TABLE_WIDTH;
    constexpr float Z_MIN = BASE_AXIS_HEIGHT;
    constexpr float Z_MAX = BASE_AXIS_HEIGHT + AXIS_RADIUS;
    std::uniform_real_distribution<float> dist_x(X_MIN, X_MAX);
    std::uniform_real_distribution<float> dist_z(Z_MIN, Z_MAX);

    // 하나의 랜덤 도착 위치 생성
    float arrival_x = dist_x(gen);
    float arrival_z = dist_z(gen);
    cv::Point3f arrive_pos{arrival_x, 0.0f, arrival_z};
    auto payload = Bridge::convert(arrive_pos);

    // 액추에이터 변환 위치 계산
    constexpr float h0 = BASE_AXIS_HEIGHT;
    constexpr float r = AXIS_RADIUS;
    float pivot_x = payload.x;
    float pivot_z = h0;
    float theta_rad = payload.theta * (std::numbers::pi_v<float> / 180.0f);
    float end_x = pivot_x + r * std::cos(theta_rad);
    float end_z = pivot_z + r * std::sin(theta_rad);

    // 도착 위치(빨간 점) 및 변환 위치(파란 점) 플로팅
    scatter(std::vector{arrival_x}, std::vector{arrival_z})
        ->marker("o").color("red").line_width(2);
    scatter(std::vector{end_x}, std::vector{end_z})
        ->marker("o").color("blue").line_width(2);
    // 두 점 연결선
    plot(std::vector{arrival_x, end_x}, std::vector{arrival_z, end_z})
        ->color("blue").line_width(1);

    xlabel("X (cm)");
    ylabel("Z (cm)");
    title("Arrival vs Actuator Endpoint");
    show();
    return 0;
}
