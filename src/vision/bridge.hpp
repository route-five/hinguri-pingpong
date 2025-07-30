//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <numbers>
#include <opencv2/opencv.hpp>

#include "../utils/constants.hpp"

using BridgePayload = struct BridgePayload_ {
    // linear actuator의 x 좌표
    float x;

    // 탁구 로봇의 회전 각도 (컴퓨터 의자 위치에서 본 좌표 평면 관점)
    float theta;

    // 스윙 길이 (몸통 돌리는 축)
    float swing_length;

    // 손목 각도 (탁구채를 얼마나 눕힐지)
    float wrist_angle;

    // 오른손잡이 여부, 기본값은 true
    bool use_right_hand = true;
};

namespace Bridge {
    constexpr float rad = 180.0f / std::numbers::pi_v<float>;

    /**
     * h0 = 탁구 로봇 축 자체 높이
     * r = 탁구 로봇 구동 반지름
     * x_p = 탁구공의 예상 도착 위치 x 좌표
     * z_p = 탁구공의 예상 도착 위치 z 좌표
     *
     * 탁구 로봇의 x, θ는 다음과 같음. (θ는 컴퓨터 의자 위치에서 본 좌표 평면 관점 θ, 오른쪽으로 90도 꺾인게 0)
     * x_p = x + r * cos(θ)
     * z_p = h0 + r * sin(θ)
     * => θ = asin((z_p - h0) / r)
     * => x = x_p - r * cos(θ)
     *
     * 어려운 것은, 탁구공을 얼마나 스윙을 길게 할지에 관한 (몸통 돌리는 축) 인자와 탁구채를 얼마나 눕힐지에 관한 (손목 축) 인자 계산
     */
    inline BridgePayload convert(const cv::Point3f& arrive_pos) {
        constexpr float h0 = BASE_AXIS_HEIGHT;
        constexpr float r = AXIS_RADIUS;
        const auto& [x_p, _, z_p] = arrive_pos;

        const bool use_right_hand = x_p >= TABLE_WIDTH / 2;

        float theta = std::asin((z_p - h0) / r);
        if (!use_right_hand)
            theta = std::numbers::pi_v<float> - theta; // x_p가 테이블의 오른쪽에 있을 때, θ를 반전

        const float x = x_p - r * std::cos(theta);

        return {
            x,
            theta * rad,
            120.0f, // 스윙 길이 (몸통 돌리는 축) - TODO: 현재는 120도(-90도 ~ 30도)로 설정, 추후 계산 필요
            35.0f, // 손목 각도 (탁구채를 얼마나 눕힐지) - TODO: 현재는 35도로 설정, 추후 계산 필요
            use_right_hand
        };
    }

    // TODO: matplot++로 테스트하기
}

#endif //BRIDGE_HPP
