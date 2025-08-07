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
    inline int to_dynamixel_unit_from_deg(float deg) {
        return static_cast<int>(std::round(1024.0f * (deg + 90.0f) / 90.0f)) % 4096;
    }

    struct Step {
        float bot_angle;
        float mid_angle;
        float top_angle;
        float upper_angle;
    };

    struct Payload {
        float x;
        std::array<Step, 5> steps; // 각 액션에 대한 동작 정보
    };

    /**
     * ID 2: 맨 아래. 0도: 탁구대 +x방향 0도 근처
     * ID 3: 중앙. 0도: 탁구대 +y방향. 60도 ~ 120도
     * ID 4: 중앙 위. 0도: 위 지지대와 아래 지지대가 평행하게 되는, 위 지지대가 뒤로 꺾이는 방향이 +
     * ID 6: 맨 위. 0도: 지지대쪽 방향으로부터 멀어질수록 +
     */
    Payload actions(const cv::Point3f& arrive_pos) {
        Payload payload{};

        // TODO: bot angle은 어떻게 할지 고민
        payload.x = arrive_pos.x;
        payload.steps[0] = Step{90, 120, -80, 50};
        payload.steps[1] = Step{90, 90, -80, 90};
        payload.steps[2] = Step{90, 60, 0, 90};
        payload.steps[3] = Step{90, 120, -80, 90};
        payload.steps[4] = Step{90, 120, -80, 50};

        return payload;
    }

    float last_pos = TABLE_WIDTH / 2;

    class SidePayload {
    public:
        // linear actuator의 x 좌표
        float x{};

        // 탁구 로봇의 회전 각도 (컴퓨터 의자 위치에서 본 좌표 평면 관점)
        float theta{};

        // 스윙 시작 각도 (몸통 돌리는 축)
        float swing_start{};

        // 스윙 끝 각도
        float swing_end{};

        // 오른손잡이 여부, 기본값은 true
        bool use_right_hand = true;

        SidePayload(
            const float x,
            const float theta,
            const float swing_start,
            const float swing_end,
            const bool use_right_hand = true
        ) : x{x},
            theta{theta}, // top
            swing_start{swing_start}, // bot
            swing_end{swing_end}, // bot
            use_right_hand{use_right_hand} {
        }

        SidePayload() = default;

        ~SidePayload() = default;
    };

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
    SidePayload convert(const cv::Point3f& arrive_pos, const cv::Vec3f& arrive_speed) {
        constexpr float h0 = BASE_AXIS_HEIGHT;
        constexpr float r = AXIS_RADIUS;
        constexpr float pi = std::numbers::pi_v<float>;

        const auto& [x_p, _, z_p] = arrive_pos;

        const bool use_right_hand = x_p >= last_pos;

        // TODO: arrive_speed를 이용해서 스윙 시작과 끝 각도를 계산하는 로직 추가
        float theta = std::asin(std::clamp((z_p - h0) / r, -1.0f, 1.0f));
        float swing_start = use_right_hand ? 45 : 135;
        float swing_end = use_right_hand ? 135 : 45;

        if (!use_right_hand) {
            theta = pi - theta; // x_p가 테이블의 오른쪽에 있을 때, θ를 반전
            /*swing_start = pi - swing_start;
            swing_end = pi - swing_end;
            wrist_angle = pi - wrist_angle;*/
        }

        const float x = x_p - r * std::cos(theta);
        last_pos = x;

        return {
            x,
            theta * rad,
            swing_start,
            swing_end,
            use_right_hand
        };
    }
}

#endif //BRIDGE_HPP
