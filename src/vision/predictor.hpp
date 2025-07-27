//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <opencv2/opencv.hpp>
#include "camera_type.hpp"
#include "../utils/constants.hpp"

using PositionWithTime = struct PositionWithTime {
    cv::Point3f position;
    std::chrono::time_point<std::chrono::steady_clock> time;
};

class Predictor {
    // TODO: Predictor
    //  2. stereo calibration으로 3D 위치 추정하기까지 구현하기
    //  3. 궤적 회귀 - quadratic_regression.cpp 로부터 살짝 수정해서 가져오기
    //  4. Kalman filter로 보정

    // TODO: 각 카메라의 fps가 달라서 얻어지는 위치 차이를 보정하는 로직 필요
    //  - 예를 들어, 왼쪽 카메라가 30fps, 오른쪽 카메라가 60fps인 경우
    //  - 왼쪽 카메라에서 공의 위치를 얻은 후, 오른쪽 카메라에서 공의 위치를 얻기까지의 시간 차이를 보정해야 함

private:
    cv::Mat R_left, R_right;
    cv::Mat t_left, t_right;
    cv::Mat P_left, P_right;

    cv::Point2f now_pt_left, now_pt_right;
    cv::Point2f prev_pt_left, prev_pt_right;
    std::chrono::time_point<std::chrono::steady_clock> t_prev_left, t_prev_right, t_now_left, t_now_right;

    std::deque<PositionWithTime> world_positions;

    void add_world_pos(const cv::Point3f& pos) {
        world_positions.emplace_back(pos, std::chrono::steady_clock::now());
        if (world_positions.size() > 10) {
            world_positions.pop_front();
        }
    }

public:
    explicit Predictor() {
        cv::FileStorage fs_left(CameraType::LEFT.projection_matrix_path(), cv::FileStorage::READ);
        fs_left["R"] >> R_left;
        fs_left["t"] >> t_left;
        fs_left["projection_matrix"] >> P_left;
        fs_left.release();

        cv::FileStorage fs_right(CameraType::RIGHT.projection_matrix_path(), cv::FileStorage::READ);
        fs_right["R"] >> R_right;
        fs_right["t"] >> t_right;
        fs_right["projection_matrix"] >> P_right;
        fs_right.release();

        const auto now = std::chrono::steady_clock::now();
        t_prev_left = now;
        t_prev_right = now;
        t_now_left = now;
        t_now_right = now;
    }

    ~Predictor() = default;

    void set_point_left(const cv::Point2f& left) {
        prev_pt_left = now_pt_left;
        t_prev_left = t_now_left;

        now_pt_left = left;
        t_now_left = std::chrono::steady_clock::now();
    }

    void set_point_right(const cv::Point2f& right) {
        prev_pt_right = now_pt_right;
        t_prev_right = t_now_right;

        now_pt_right = right;
        t_now_right = std::chrono::steady_clock::now();
    }

    [[nodiscard]] const cv::Point2f& get_left_point() const {
        return now_pt_left;
    }

    [[nodiscard]] const cv::Point2f& get_right_point() const {
        return now_pt_right;
    }

    [[nodiscard]] const cv::Point2f& get_prev_left_point() const {
        return prev_pt_left;
    }

    [[nodiscard]] const cv::Point2f& get_prev_right_point() const {
        return prev_pt_right;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_prev_left_time() const {
        return t_prev_left;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_prev_right_time() const {
        return t_prev_right;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_now_left_time() const {
        return t_now_left;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_now_right_time() const {
        return t_now_right;
    }

    /**
     * @brief 현재 시간과 이전 시간 사이의 시간 차이를 초 단위로 반환합니다.
     */
    [[nodiscard]] auto get_left_dt() const {
        return std::chrono::duration<float>(t_now_left - t_prev_left).count();
    }

    /**
     * @brief 현재 시간과 이전 시간 사이의 시간 차이를 초 단위로 반환합니다.
     */
    [[nodiscard]] auto get_right_dt() const {
        return std::chrono::duration<float>(t_now_right - t_prev_right).count();
    }

    /**
     * @brief 두 카메라 projection 행렬을 사용해 2D 대응점으로부터 삼각측량으로 3D 위치를 계산합니다.
     * @param pt_left 왼쪽 카메라에서의 2D 점 (픽셀 좌표계).
     * @param pt_right 오른쪽 카메라에서의 2D 점 (픽셀 좌표계).
     * @return 계산된 3D 좌표(Pinhole world coordinates, 동차 좌표 정규화 완료).
     * @note pt_left와 pt_right는 동일한 물체에 대응되는 2D 좌표여야 합니다.
     */
    [[nodiscard]] cv::Point3f get_world_pos_from_camera_pos(
        const cv::Point2f& pt_left,
        const cv::Point2f& pt_right
    ) const {
        const cv::Mat pts1(1, 1, CV_32FC2, cv::Scalar(pt_left.x, pt_left.y));
        const cv::Mat pts2(1, 1, CV_32FC2, cv::Scalar(pt_right.x, pt_right.y));

        cv::Mat pts4d;
        cv::triangulatePoints(P_left, P_right, pts1, pts2, pts4d);

        cv::Mat p = pts4d.col(0);
        p /= p.at<float>(3); // 동차 좌표 정규화

        return {p.at<float>(0), p.at<float>(1), p.at<float>(2)};
    }

    /**
     * @brief 주어진 3D 월드 좌표를 카메라 좌표로 변환합니다.
     * @param world_pos 월드 좌표 (cm 단위).
     * @param camera_type 카메라 타입 (LEFT, RIGHT 등).
     * @return 카메라 좌표 (픽셀 좌표계).
     */
    [[nodiscard]] cv::Point2f get_camera_pos_from_world_pos(
        const cv::Point3f& world_pos,
        const CameraType& camera_type
    ) const {
        const cv::Mat pt3d_h = (cv::Mat_<double>(4, 1) << world_pos.x, world_pos.y, world_pos.z, 1.0f);

        // TODO: CameraType::TOP
        const cv::Mat& P = (camera_type == CameraType::LEFT) ? P_left : P_right;
        cv::Mat projected = P * pt3d_h;

        float x = projected.at<double>(0, 0) / projected.at<double>(2, 0);
        float y = projected.at<double>(1, 0) / projected.at<double>(2, 0);
        return {x, y};
    }

    /**
     * @brief 현재 왼쪽과 오른쪽 카메라의 2D 점을 사용하여 새로운 월드 좌표를 계산하고 저장합니다.
     * @return 계산된 3D 월드 좌표 (cm 단위).
     */
    [[nodiscard]] cv::Point3f get_new_world_pos(
        const float x_lim = TABLE_WIDTH,
        const float y_lim = TABLE_HEIGHT
    ) {
        const auto ret = get_world_pos_from_camera_pos(now_pt_left, now_pt_right);
        add_world_pos(ret);
        return ret;
    }

    /**
     * @brief 현재까지 기록된 월드 위치의 개수를 반환합니다.
     * @return 월드 위치의 개수.
     * @note 최대 10개의 위치를 저장하며, 가장 오래된 위치는 자동으로 제거됩니다.
     */
    std::size_t get_world_positions_size() const {
        return world_positions.size();
    }

    /**
     * @brief 현재 위치와 이전 위치를 기반으로 속도를 계산합니다.
     * @return cm/s 단위의 속도 벡터 (x, y, z).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] cv::Vec3f get_world_speed() {
        assert(world_positions.size() >= 2 && "속도를 계산하기 위해서는 최소 2개의 위치가 필요합니다.");

        const auto [prev_position, prev_time] = world_positions[world_positions.size() - 2];
        const auto [now_position, now_time] = world_positions[world_positions.size() - 1];
        const auto dt = std::chrono::duration<float>(now_time - prev_time).count();

        return (now_position - prev_position) / dt;
    }

    /**
     * @brief 현재 위치를 반환합니다.
     * @note get_world_positions_size() 으로 1개 이상의 위치가 있는지 확인해야 합니다.
     * @return cm 단위의 현재 3D 위치 (x, y, z).
     */
    [[nodiscard]] cv::Point3f get_world_pos() const {
        assert(world_positions.size() >= 1 && "현재 위치를 가져오기 위해서는 최소 1개의 위치가 필요합니다.");

        return world_positions[world_positions.size() - 1].position;
    }

    // TODO: 매 world_pos 측정 때마다 (네트 넘기 전까지) 계속 predict_world_pos로 예측 궤적을 수정해야함?

    // TODO: 아래의 세 함수는 탁구 로봇 입장에서 공을 던지는 관점으로 코딩됨. 실제론 반대이므로 수정 필요.

    /**
     * @brief 미래의 위치를 예측합니다.
     * @param world_pos 현재의 3D 위치 (cm 단위).
     * @param world_speed 현재의 3D 속도 (cm/s 단위).
     * @param dt 현재로부터 예측할 시간 (초 단위).
     * @return 현재로부터 dt 초 후의 예측된 3D 위치 (cm 단위).
     * @note 예측 시간 dt는 0 이상 PREDICT_MAX_TIME 이하이어야 합니다.
     */
    [[nodiscard]] static std::optional<cv::Point3f> predict_world_pos(
        const cv::Point3f& world_pos,
        const cv::Vec3f& world_speed,
        const float dt
    ) {
        if (PREDICT_MIN_TIME <= dt && dt <= PREDICT_MAX_TIME) {
            const auto vx = world_speed[0];
            const auto vy = world_speed[1];
            const auto vz = world_speed[2];

            const auto x = world_pos.x + vx * dt;
            const auto y = world_pos.y + vy * dt;

            float z = world_pos.z;
            float vz_local = vz;
            float t_remaining = dt;

            int iter_left = PREDICT_MAX_ITERATIONS;
            while (iter_left--) {
                const float a = -0.5f * GRAVITY;
                const float b = vz_local;
                const float c = z;

                const float D = b * b - 4 * a * c;
                if (D < 0) {
                    return std::nullopt; // 실수 해가 없음, 즉 공이 바닥에 닿지 않음
                }

                // TODO: t_bounce < 0인 경우 조사 및 연구 필요
                const float t_bounce = (-b - std::sqrt(D)) / (2 * a);

                if (t_bounce >= t_remaining || t_bounce <= 0) {
                    if (t_bounce <= 0)
                        std::cout << "Warning: t_bounce is non-positive, using remaining time instead."
                            << std::endl;
                    z = z + vz_local * t_remaining + a * t_remaining * t_remaining * 2;
                    return cv::Point3f{x, y, z};
                }
                else {
                    t_remaining -= t_bounce;
                    vz_local = TABLE_BOUNCE_COEFFICIENT * -vz_local;
                    z = 0.0f;
                }
            }

            return std::nullopt; // 반복이 너무 많음
        }

        return std::nullopt; // 공이 너무 빠르거나 너무 느려서 예측할 수 없음
    }

    /**
     * @brief 현재 위치와 속도를 기반으로 x_lim, y_lim에 도달하는 시간을 계산합니다.
     * @param x_lim 현재 위치에서 도달할 x 좌표의 한계 (cm 단위).
     * @param y_lim 현재 위치에서 도달할 y 좌표의 한계 (cm 단위).
     * @return 현재 위치에서 x_lim 또는 y_lim에 도달하는 예상 시간 (초 단위).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] float get_arrive_time(const float x_lim = TABLE_WIDTH, const float y_lim = TABLE_HEIGHT) {
        const auto world_pos = get_world_pos();
        const auto world_speed = get_world_speed();

        float t_x = world_speed[0] == 0 ? std::numeric_limits<float>::max() : (x_lim - world_pos.x) / world_speed[0];
        if (t_x < 0) t_x = std::numeric_limits<float>::max(); // 이미 x_lim을 지나쳤다면 무한대

        float t_y = world_speed[1] == 0 ? std::numeric_limits<float>::max() : (y_lim - world_pos.y) / world_speed[1];
        if (t_y < 0) t_y = std::numeric_limits<float>::max(); // 이미 y_lim을 지나쳤다면 무한대

        return std::min(t_x, t_y);
    }

    /**
     * @brief 현재 위치와 속도를 기반으로 x_lim, y_lim에 도달하는 위치를 예측합니다.
     * @param x_lim 현재 위치에서 도달할 x 좌표의 한계 (cm 단위).
     * @param y_lim 현재 위치에서 도달할 y 좌표의 한계 (cm 단위).
     * @return 예측된 3D 위치 (cm 단위).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] std::optional<cv::Point3f> get_arrive_pos(const float x_lim = TABLE_WIDTH,
                                                            const float y_lim = TABLE_HEIGHT) {
        const auto world_pos = get_world_pos();
        const auto world_speed = get_world_speed();
        const float t_arrive = get_arrive_time(x_lim, y_lim);

        return predict_world_pos(world_pos, world_speed, t_arrive);
    }
};

#endif //PREDICTOR_H
