//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <numbers>
#include <opencv2/opencv.hpp>

#include "../utils/constants.hpp"

namespace Bridge {
    constexpr float pi = std::numbers::pi_v<float>;
    constexpr float rad = 180.0f / pi;

    /**
     * 0 -> -90도
     * 1024 -> 0도
     * 2048 -> 90도
     * 3072 -> 180도
     */
    inline int to_dynamixel_unit_from_deg(const float deg) {
        return static_cast<int>(std::round(1024.0f * (deg + 90.0f) / 90.0f)) % 4096;
    }

    class Payload {
    public:
        // linear actuator의 x 좌표
        float x{};

        // 탁구 로봇의 회전 각도 (컴퓨터 의자 위치에서 본 좌표 평면 관점)
        float theta{};

        // 스윙 시작 각도 (몸통 돌리는 축)
        float swing_start{};

        // 스윙 끝 각도 
        float swing_end{};

        // 손목 각도 (탁구채를 얼마나 눕힐지)
        float wrist_angle{};

        // 오른손잡이 여부, 기본값은 true
        bool use_right_hand = true;

        Payload(const float x, const float theta, const float swing_start, const float swing_end,
                const float wrist_angle, const bool use_right_hand = true)
            : x{x},
              theta{90 - theta}, // mid
              swing_start{swing_start}, // bot
              swing_end{swing_end}, // bot
              wrist_angle{90 - wrist_angle}, // top
              use_right_hand{use_right_hand} {
        }

        Payload() = default;

        ~Payload() = default;
    };

    // inline float swing_start(const cv::Point3f& )

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
    inline Payload convert(const cv::Point3f& arrive_pos) {
        constexpr float h0 = BASE_AXIS_HEIGHT;
        constexpr float r = AXIS_RADIUS;
        constexpr float pi = std::numbers::pi_v<float>;

        const auto& [x_p, _, z_p] = arrive_pos;

        const bool use_right_hand = x_p >= TABLE_WIDTH / 2;

        float theta = std::asin(std::clamp((z_p - h0) / r, -1.0f, 1.0f));
        float swing_start = -pi / 3; // TODO:
        float swing_end = pi / 6; // TODO: 
        float wrist_angle = pi / 6;

        if (!use_right_hand) {
            theta = pi - theta; //x_p가 테이블의 오른쪽에 있을 때, θ를 반전
            swing_start = pi - swing_start;
            swing_end = pi - swing_end;
            wrist_angle = pi - wrist_angle;
        }

        const float x = x_p - r * std::cos(theta);

        return {
            x,
            theta * rad,
            swing_start * rad,
            swing_end * rad,
            wrist_angle * rad,
            use_right_hand
        };
    }

    // TODO: matplot++로 테스트하기
}

#endif //BRIDGE_HPP
