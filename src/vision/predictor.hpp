//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <opencv2/opencv.hpp>
#include <array>
#include "camera_type.hpp"
#include "../utils/constants.hpp"
#include "../utils/log.hpp"

using PositionWithTime = struct PositionWithTime {
    cv::Point3f position;
    std::chrono::time_point<std::chrono::steady_clock> time;
};

class Predictor {
    // TODO: stereo calibration으로부터 depth 구하기

private:
    cv::KalmanFilter kalman_filter;

    cv::UMat R_left, R_right, R_top;
    cv::UMat t_left, t_right, t_top;
    cv::UMat P_left, P_right, P_top;
    cv::UMat K_left, K_right, K_top;
    cv::UMat dist_coeffs_left, dist_coeffs_right, dist_coeffs_top;

    std::optional<cv::Point2f> now_pt_left, now_pt_right, now_pt_top;
    std::optional<cv::Point2f> prev_pt_left, prev_pt_right, prev_pt_top;
    bool is_initialized_left = false,
         is_initialized_right = false,
         is_initialized_top = false;
    std::chrono::time_point<std::chrono::steady_clock>
        t_prev_left, t_prev_right, t_prev_top, t_now_left, t_now_right, t_now_top;

    std::deque<PositionWithTime> world_positions;

public:
    explicit Predictor() : kalman_filter(6, 3) {
        cv::FileStorage fs_left(CameraType::LEFT.projection_matrix_path(), cv::FileStorage::READ);
        fs_left["R"] >> R_left;
        fs_left["t"] >> t_left;
        fs_left["projection_matrix"] >> P_left;
        fs_left.release();

        cv::FileStorage fs_left_2(CameraType::LEFT.calibration_matrix_path(), cv::FileStorage::READ);
        fs_left_2["camera_matrix"] >> K_left;
        fs_left_2["dist_coeffs"] >> dist_coeffs_left;
        fs_left_2.release();

        cv::FileStorage fs_right(CameraType::RIGHT.projection_matrix_path(), cv::FileStorage::READ);
        fs_right["R"] >> R_right;
        fs_right["t"] >> t_right;
        fs_right["projection_matrix"] >> P_right;
        fs_right.release();

        cv::FileStorage fs_right_2(CameraType::RIGHT.calibration_matrix_path(), cv::FileStorage::READ);
        fs_right_2["camera_matrix"] >> K_right;
        fs_right_2["dist_coeffs"] >> dist_coeffs_right;
        fs_right_2.release();

        cv::FileStorage fs_top(CameraType::TOP.projection_matrix_path(), cv::FileStorage::READ);
        fs_top["R"] >> R_top;
        fs_top["t"] >> t_top;
        fs_top["projection_matrix"] >> P_top;
        fs_top.release();

        cv::FileStorage fs_top_2(CameraType::TOP.calibration_matrix_path(), cv::FileStorage::READ);
        fs_top_2["camera_matrix"] >> K_top;
        fs_top_2["dist_coeffs"] >> dist_coeffs_top;
        fs_top_2.release();

        const auto now = std::chrono::steady_clock::now();
        t_prev_left = now;
        t_prev_right = now;
        t_prev_top = now;
        t_now_left = now;
        t_now_right = now;
        t_now_top = now;

        kalman_filter.init(6, 3); // 6 state variables: x, y, z, vx, vy, vz; 3 measurements: x, y, z
        kalman_filter.transitionMatrix = (cv::Mat_<float>(6, 6) <<
            1, 0, 0, 1, 0, 0,
            0, 1, 0, 0, 1, 0,
            0, 0, 1, 0, 0, 1,
            0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 1);
        cv::setIdentity(kalman_filter.measurementMatrix); // 3x6
        cv::setIdentity(kalman_filter.processNoiseCov, cv::Scalar::all(1e-4));
        cv::setIdentity(kalman_filter.measurementNoiseCov, cv::Scalar::all(1e-1));
        cv::setIdentity(kalman_filter.errorCovPost, cv::Scalar::all(1));
        kalman_filter.statePost.setTo(cv::Scalar(0));
    }

    ~Predictor() = default;

    void add_world_pos(const cv::Point3f& pos) {
        world_positions.emplace_back(pos, std::chrono::steady_clock::now());
        if (world_positions.size() > PREDICTOR_HISTORY_N) {
            world_positions.pop_front();
        }
    }

    void set_point_left(const std::optional<cv::Point2f>& left) {
        is_initialized_left = true;
        prev_pt_left = now_pt_left;
        t_prev_left = t_now_left;

        now_pt_left = left;
        t_now_left = std::chrono::steady_clock::now();
    }

    void set_point_right(const std::optional<cv::Point2f>& right) {
        is_initialized_right = true;
        prev_pt_right = now_pt_right;
        t_prev_right = t_now_right;

        now_pt_right = right;
        t_now_right = std::chrono::steady_clock::now();
    }

    void set_point_top(const std::optional<cv::Point2f>& top) {
        is_initialized_top = true;

        prev_pt_top = now_pt_top;
        t_prev_top = t_now_top;

        now_pt_top = top;
        t_now_top = std::chrono::steady_clock::now();
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_left_point() const {
        return now_pt_left;
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_right_point() const {
        return now_pt_right;
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_top_point() const {
        return now_pt_top;
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_prev_left_point() const {
        return prev_pt_left;
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_prev_right_point() const {
        return prev_pt_right;
    }

    [[nodiscard]] const std::optional<cv::Point2f>& get_prev_top_point() const {
        return prev_pt_top;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_prev_left_time() const {
        return t_prev_left;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_prev_right_time() const {
        return t_prev_right;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_prev_top_time() const {
        return t_prev_top;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_now_left_time() const {
        return t_now_left;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_now_right_time() const {
        return t_now_right;
    }

    [[nodiscard]] const std::chrono::time_point<std::chrono::steady_clock>& get_now_top_time() const {
        return t_now_top;
    }

    /**
     * @brief 왼쪽 카메라의 현재 시간과 이전 시간 사이의 시간 차이를 초 단위로 반환합니다.
     */
    [[nodiscard]] auto get_left_dt() const {
        return std::chrono::duration<float>(t_now_left - t_prev_left).count();
    }

    /**
     * @brief 오른쪽 카메라의 현재 시간과 이전 시간 사이의 시간 차이를 초 단위로 반환합니다.
     */
    [[nodiscard]] auto get_right_dt() const {
        return std::chrono::duration<float>(t_now_right - t_prev_right).count();
    }

    /**
     * @brief 위쪽 카메라의 현재 시간과 이전 시간 사이의 시간 차이를 초 단위로 반환합니다.
     */
    [[nodiscard]] auto get_top_dt() const {
        return std::chrono::duration<float>(t_now_top - t_prev_top).count();
    }

    /**
     * @brief 두 카메라 projection 행렬을 사용해 2D 대응점으로부터 삼각측량으로 3D 위치를 계산합니다.
     * @param pt_left 왼쪽 카메라에서의 2D 점 (픽셀 좌표계).
     * @param pt_right 오른쪽 카메라에서의 2D 점 (픽셀 좌표계).
     * @return 계산된 3D 좌표(Pinhole world coordinates, 동차 좌표 정규화 완료).
     * @note pt_left와 pt_right는 동일한 물체에 대응되는 2D 좌표여야 합니다.
     */
    [[nodiscard]] inline cv::Point3f
    triangulate(const cv::Point2f& pt_left, const cv::Point2f& pt_right) const noexcept {
        // Avoid unnecessary cv::UMat allocations and use cv::Vec4f directly
        float pts1_data[2] = {pt_left.x, pt_left.y};
        float pts2_data[2] = {pt_right.x, pt_right.y};
        const cv::Mat pts1(1, 1, CV_32FC2, pts1_data);
        const cv::Mat pts2(1, 1, CV_32FC2, pts2_data);

        cv::Mat pts4d;
        cv::triangulatePoints(P_left, P_right, pts1, pts2, pts4d);

        // Use cv::Vec4f to avoid creating a new Mat and .col()
        cv::Vec4f p(
            pts4d.at<float>(0),
            pts4d.at<float>(1),
            pts4d.at<float>(2),
            pts4d.at<float>(3)
        );
        return {p[0] / p[3], p[1] / p[3], p[2] / p[3]};
    }

    /**
     * @brief Top 카메라의 픽셀 좌표를 탁구대 기준 평면(z=z_plane)에서의 3D 월드 좌표로 변환합니다.
     * @param pt 왜곡된 이미지 평면 상에서의 픽셀 좌표
     * @param z_plane 교차할 평면의 z 값 (기본값: 0.0f)
     * @return 탁구대 기준 평면(z=z_plane)에서의 3D 월드 좌표
     */
    [[nodiscard]] inline cv::Point3f birds_eye_view(const cv::Point2f& pt, const float z_plane = 0.0f) const noexcept {
        // Avoid heap allocations, use cv::Vec3d and stack memory
        const std::array<cv::Point2f, 1> src{pt};
        std::array<cv::Point2f, 1> dst;
        cv::undistortPoints(src, dst, K_top, dist_coeffs_top);
        const cv::Point2f& pt_undistorted = dst[0];
        // Use cv::Vec3d for ray vector
        const cv::Vec3d ray_cam{pt_undistorted.x, pt_undistorted.y, 1.0};
        // Use cv::Matx33d and cv::Vec3d for R_top and t_top for performance (if possible, preconvert at load time)
        cv::Matx33d R_topx;
        R_top.convertTo(R_topx, CV_64F);
        cv::Vec3d t_topx;
        t_top.convertTo(t_topx, CV_64F);
        // ray_world = R_top.t() * ray_cam;
        cv::Vec3d ray_world = R_topx.t() * ray_cam;
        cv::Vec3d origin_world = -R_topx.t() * t_topx;

        // z=z_plane 평면과의 교차점 계산
        const double s = (z_plane - origin_world[2]) / ray_world[2];
        cv::Vec3d world_pos = origin_world + s * ray_world;

        return {
            static_cast<float>(world_pos[0]),
            static_cast<float>(world_pos[1]),
            static_cast<float>(world_pos[2])
        };
    }

    static std::pair<float, float> linear_regression(
        const std::vector<float>& indices,
        const std::vector<float>& values
    ) {
        const auto n = static_cast<float>(indices.size());
        if (n == 0) return {0.0f, 0.0f};

        float sum_x = 0.0f, sum_y = 0.0f, sum_xx = 0.0f, sum_xy = 0.0f;
        for (size_t i = 0; i < indices.size(); ++i) {
            sum_x += indices[i];
            sum_y += values[i];
            sum_xx += indices[i] * indices[i];
            sum_xy += indices[i] * values[i];
        }

        const float denom = n * sum_xx - sum_x * sum_x;
        if (denom == 0.0f) return {0.0f, values.empty() ? 0.0f : values.back()};

        float slope = (n * sum_xy - sum_x * sum_y) / denom;
        float intercept = (sum_y * sum_xx - sum_x * sum_xy) / denom;
        return {slope, intercept};
    };

    /**
     * @brief 세 카메라의 projection 행렬을 사용해 2D 대응점으로부터 3D 위치를 계산합니다.
     * @param pt_left 왼쪽 카메라에서의 2D 점 (픽셀 좌표계)
     * @param pt_right 오른쪽 카메라에서의 2D 점 (픽셀 좌표계)
     * @param pt_top 위쪽 카메라에서의 2D 점 (픽셀 좌표계). 생략 시 삼각측량으로만 구함
     * @param dt 프레임 간 시간 차이 (초 단위). 현재 시간과 이전 시간 사이의 차이를 사용합니다.
     * @return 계산된 3D 좌표
     * @note pt_left와 pt_right, pt_top은 동일한 물체에 대응되는 2D 좌표여야 합니다. 또한, pt_top은 선택 사항입니다.
     */
    [[nodiscard]] std::optional<cv::Point3f> pos_2d_to_3d(
        const std::optional<cv::Point2f>& pt_left,
        const std::optional<cv::Point2f>& pt_right,
        const std::optional<cv::Point2f>& pt_top,
        const float dt
    ) const noexcept {
        if (pt_left.has_value() && pt_right.has_value()) {
            const auto world_pos_lr = triangulate(pt_left.value(), pt_right.value());

            // 탁구공이 탁구대 범위 내에 있는 경우 = 공을 던진 것, z는 삼각측량의 값을 사용하고, x, y는 선형 회귀로 보정 (등속 운동이므로)
            static std::deque<std::pair<float, float>> top_xy_history;

            if (pt_top.has_value()) {
                const auto world_pos_top = birds_eye_view(pt_top.value(), world_pos_lr.z);

                // Add new point to history
                top_xy_history.emplace_back(world_pos_top.x, world_pos_top.y);
                if (static_cast<int>(top_xy_history.size()) > TOP_HISTORY_N)
                    top_xy_history.pop_front();

                // On-the-fly regression without allocations
                const size_t N = top_xy_history.size();
                float sum_i = 0.f, sum_i2 = 0.f;
                float sum_x = 0.f, sum_ix = 0.f;
                float sum_y = 0.f, sum_iy = 0.f;
                for (size_t i = 0; i < N; ++i) {
                    const float x_i = top_xy_history[i].first;
                    const float y_i = top_xy_history[i].second;
                    sum_i += static_cast<float>(i);
                    sum_i2 += static_cast<float>(i * i);
                    sum_x += x_i;
                    sum_ix += x_i * static_cast<float>(i);
                    sum_y += y_i;
                    sum_iy += y_i * static_cast<float>(i);
                }

                const auto n = static_cast<float>(N);
                const float denom = n * sum_i2 - sum_i * sum_i;
                const float slope_x = (denom != 0.f) ? (n * sum_ix - sum_i * sum_x) / denom : 0.f;
                const float intercept_x = (denom != 0.f) ? (sum_x * sum_i2 - sum_i * sum_ix) / denom : sum_x / n;
                const float slope_y = (denom != 0.f) ? (n * sum_iy - sum_i * sum_y) / denom : 0.f;
                const float intercept_y = (denom != 0.f) ? (sum_y * sum_i2 - sum_i * sum_iy) / denom : sum_y / n;
                const float t = n - 1.f;

                return cv::Point3f{
                    slope_x * t + intercept_x,
                    slope_y * t + intercept_y,
                    world_pos_lr.z
                };
            }

            // 탁구공이 탁구대 범위 밖에 있는 경우, 공을 던진게 아니라 들고 있거나 의미 없는 경우이므로 보정 없이 그대로 반환
            return world_pos_lr;
        }

        return std::nullopt;

        // // 삼각측량 결과가 탁구대 범위 내에 있는 경우 = 공을 던진 것이므로 kalman filter + 중력 보정
        // if (0 <= world_pos_lr.x && world_pos_lr.x <= TABLE_WIDTH &&
        //     0 <= world_pos_lr.y && world_pos_lr.y <= TABLE_HEIGHT &&
        //     0 <= world_pos_lr.z) {
        //     // update transition matrix with current dt
        //     kalman_filter.transitionMatrix = (cv::Mat_<float>(6, 6) <<
        //         1, 0, 0, dt, 0, 0,
        //         0, 1, 0, 0, dt, 0,
        //         0, 0, 1, 0, 0, dt,
        //         0, 0, 0, 1, 0, 0,
        //         0, 0, 0, 0, 1, 0,
        //         0, 0, 0, 0, 0, 1);
        //
        //     // predict with gravity correction
        //     cv::UMat prediction = kalman_filter.predict();
        //     prediction.at<float>(2) -= 0.5f * GRAVITY * dt * dt; // z -= 0.5 * g * dt^2
        //     prediction.at<float>(5) -= GRAVITY * dt; // v_z -= g * dt
        //
        //     // correct with measurement
        //     const cv::UMat measurement = (cv::Mat_<float>(3, 1) << world_pos_lr.x, world_pos_lr.y, world_pos_lr.z);
        //     cv::UMat estimated = kalman_filter.correct(measurement);
        //
        //     return {
        //         estimated.at<float>(0),
        //         estimated.at<float>(1),
        //         estimated.at<float>(2)
        //     };
        // }
        // // 삼각측량 결과가 탁구대 범위 밖에 있는 경우, 공을 던진게 아니라 들고 있거나 의미 없는 경우이므로 kalman filter 보정 없이 그대로 반환
        // else {
        //     return world_pos_lr;
        // }
    }

    /**
     * @brief 주어진 3D 월드 좌표를 카메라 좌표로 변환합니다.
     * @param world_pos 월드 좌표 (cm 단위)
     * @param camera_type 카메라 타입 (LEFT, RIGHT, TOP 등)
     * @return 카메라 좌표 (픽셀 좌표계).
     */
    [[nodiscard]] cv::Point2f pos_3d_to_2d(
        const cv::Point3f& world_pos,
        const CameraType& camera_type
    ) const {
        std::array<cv::Point3f, 1> objectPoints{world_pos};
        std::array<cv::Point2f, 1> image_points;

        cv::UMat R, t, K, dist_coeffs;
        if (camera_type == CameraType::LEFT) {
            R = R_left;
            t = t_left;
            K = K_left;
            dist_coeffs = dist_coeffs_left;
        }
        else if (camera_type == CameraType::RIGHT) {
            R = R_right;
            t = t_right;
            K = K_right;
            dist_coeffs = dist_coeffs_right;
        }
        else if (camera_type == CameraType::TOP) {
            R = R_top;
            t = t_top;
            K = K_top;
            dist_coeffs = dist_coeffs_top;
        }
        else {
            throw std::runtime_error("Unsupported camera type in pos_3d_to_2d");
        }

        cv::projectPoints(objectPoints, R, t, K, dist_coeffs, image_points);
        return image_points[0];
    }

    /**
     * @brief 현재 왼쪽과 오른쪽, 위쪽의 카메라의 2D 점을 사용하여 새로운 월드 좌표를 계산하고 저장합니다. 월드 좌표가 계산되지 않은 경우 저장하지 않습니다.
     * @param fallback_dt 이전에 저장된 월드 위치가 없을 때 사용할 시간 간격 (초 단위)
     * @param use_top 위쪽 카메라의 2D 점을 사용할지 여부 (기본값: true)
     * @return 계산된 3D 월드 좌표 (cm 단위)
     */
    [[nodiscard]] std::optional<cv::Point3f> get_new_world_pos(const float fallback_dt, const bool use_top = true) {
        const auto now = std::chrono::steady_clock::now();
        const auto ret = pos_2d_to_3d(
            now_pt_left,
            now_pt_right,
            use_top ? now_pt_top : std::nullopt,
            world_positions.empty()
                ? fallback_dt
                : std::chrono::duration<float>(now - world_positions.back().time).count()
        );

        if (ret.has_value()) {
            add_world_pos(ret.value());
        }

        return ret;
    }

    /**
     * @brief 현재 왼쪽과 오른쪽, 위쪽의 카메라의 2D 점을 사용하여 새로운 월드 좌표를 계산하지만 저장하지 않습니다.
     * @param fallback_dt 이전에 저장된 월드 위치가 없을 때 사용할 시간 간격 (초 단위)
     * @param use_top 위쪽 카메라의 2D 점을 사용할지 여부 (기본값: true)
     * @return 계산된 3D 월드 좌표 (cm 단위)
     */
    [[nodiscard]] std::optional<cv::Point3f> get_new_world_pos_no_save(const float fallback_dt, const bool use_top = true) const {
        const auto now = std::chrono::steady_clock::now();
        const auto ret = pos_2d_to_3d(
            now_pt_left,
            now_pt_right,
            use_top ? now_pt_top : std::nullopt,
            world_positions.empty()
                ? fallback_dt
                : std::chrono::duration<float>(now - world_positions.back().time).count()
        );

        return ret;
    }

    /**
     * @brief 현재까지 기록된 월드 위치의 개수를 반환합니다.
     * @return 월드 위치의 개수.
     * @note 최대 10개의 위치를 저장하며, 가장 오래된 위치는 자동으로 제거됩니다.
     */
    [[nodiscard]] std::size_t get_world_positions_size() const {
        return world_positions.size();
    }

    /**
     * @brief 최근 N-1개의 연속 위치 쌍을 기반으로 속도를 계산합니다.
     * @return cm/s 단위의 속도 벡터 (x, y, z).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] std::optional<cv::Vec3f> get_world_speed() const {
        const int N = std::min(TOP_HISTORY_N, static_cast<int>(world_positions.size()) - 1);

        if (N >= 1) {
            std::vector<cv::Vec3f> valid_vs;

            for (int i = world_positions.size() - N - 1; i < world_positions.size() - 1; ++i) {
                const auto& [p1, t1] = world_positions[i];
                const auto& [p2, t2] = world_positions[i + 1];
                const float dt = std::chrono::duration<float>(t2 - t1).count();
                if (1e-5f < dt && dt < 0.2f) {
                    const auto v = cv::Vec3f(p2 - p1) / dt;
                    if (cv::norm(v) < 1500.0f && v[1] < 0.0f) {
                        valid_vs.push_back(v);
                    }
                }
            }

            if (!valid_vs.empty()) {
                // 기준 벡터 = 가장 최근 벡터
                const auto ref = valid_vs.back() / cv::norm(valid_vs.back());
                cv::Vec3f sum{0, 0, 0};
                int count = 0;

                for (const auto& v : valid_vs) {
                    const float cos_theta = ref.dot(v / cv::norm(v));
                    if (cos_theta > 0.5f) { // 방향이 60도 이내로 유사
                        sum += v;
                        count++;
                    }
                }

                if (count > 0) return sum / static_cast<float>(count);
            }
        }

        return std::nullopt;
    }

    /**
     * @brief 현재 위치를 반환합니다.
     * @note get_world_positions_size() 으로 1개 이상의 위치가 있는지 확인해야 합니다.
     * @return cm 단위의 현재 3D 위치 (x, y, z).
     */
    [[nodiscard]] std::optional<cv::Point3f> get_world_pos() const {
        return world_positions.empty()
                   ? std::nullopt
                   : std::make_optional(world_positions[world_positions.size() - 1].position);
    }

    /**
     * @brief 미래의 위치를 예측합니다.
     * @param init_pos 현재의 3D 위치 (cm 단위).
     * @param init_speed 현재의 3D 속도 (cm/s 단위).
     * @param t_after 현재로부터 예측할 시간 (초 단위).
     * @return 현재로부터 t_after 초 후의 예측된 3D 위치 (cm 단위).
     * @note 예측 시간 t_after는 PREDICT_MIN_TIME 이상 PREDICT_MAX_TIME 이하이어야 합니다.
     */
    [[nodiscard]] static std::optional<cv::Point3f> predict_world_pos(
        const cv::Point3f& init_pos,
        const cv::Vec3f& init_speed,
        const float t_after
    ) {
        assert(
            PREDICT_MIN_TIME <= t_after && t_after <= PREDICT_MAX_TIME &&
            "예측 시간 dt는 PREDICT_MIN_TIME 이상 PREDICT_MAX_TIME 이하이어야 합니다.");

        const auto vx = init_speed[0];
        const auto vy = init_speed[1];
        const auto vz = init_speed[2];

        const auto x = init_pos.x + vx * t_after;
        const auto y = init_pos.y + vy * t_after;

        float z = init_pos.z;
        float vz_local = vz;
        float t_remaining = t_after;

        if (z < 0)
            return std::nullopt;

        int iter_left = PREDICT_MAX_ITERATIONS;
        while (iter_left--) {
            const float D = vz_local * vz_local + 2 * GRAVITY * z;
            if (D < 0) {
                return std::nullopt; // 실수 해가 없음, 즉 공이 바닥에 닿지 않음
            }

            const float t_bounce = (vz_local + std::sqrt(D)) / GRAVITY;
            if (t_bounce < 0) {
                return std::nullopt; // 공이 이미 바닥 아래에 있음, 불가능한 케이스 - 위의 z < 0 조건으로 걸러짐
            }
            else if (t_bounce >= t_remaining) { // 남은 계산 내에 바닥에서 튕길 일이 없는 경우
                z = z + vz_local * t_remaining - 0.5f * GRAVITY * t_remaining * t_remaining;
                return cv::Point3f{x, y, z};
            }
            else if (t_bounce == 0) { // 바닥에서 튕기지 않는 경우 (너무 작은 시간)
                z = z + vz_local * t_remaining; // 바닥에서 튕기지 않고 그냥 지나감
                return cv::Point3f{x, y, z};
            }
            else {
                t_remaining -= t_bounce;
                // 초기 v_z는 +(올라가)든 -(내려가)든 상관없는데, t_bounce초가 지난 후, 위로 튀는 것만 고려하므로(여전히 v_z<0면 z=0에서 당연히 t_bounce=0, 예외상황) 그 이후부턴 v_z는 양수여야 함.
                vz_local -= GRAVITY * t_bounce;
                vz_local = std::abs(-TABLE_BOUNCE_COEFFICIENT * vz_local);
                z = 0.0f;
            }
        }

        return std::nullopt; // 반복이 너무 많음
    }

    /**
     * @brief 현재 위치와 속도를 기반으로 y=0 혹은 y=ylim 도달하는 시간을 계산합니다.
     * @param world_pos 현재 위치 (cm 단위)
     * @param world_speed 현재 속도 (cm/s 단위)
     * @param ylim y축의 끝 값
     * @return 현재 위치에서 y=0 혹은 y=ylim에 도에하는 예상 시간 (초 단위).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] static std::optional<float> get_arrive_time(
        const std::optional<cv::Point3f>& world_pos,
        const std::optional<cv::Vec3f>& world_speed,
        const float ylim = TABLE_HEIGHT
    ) {
        if (world_pos.has_value() && world_speed.has_value()) {
            //float t_ylim = world_speed.value()[1] == 0
            //                   ? std::numeric_limits<float>::max()
            //                   : (ylim - world_pos->y) / world_speed.value()[1];
            //if (t_ylim <= 0) t_ylim = std::numeric_limits<float>::max(); // 이미 지나쳤다면 무한대

            float t_y0 = world_speed.value()[1] == 0
                             ? std::numeric_limits<float>::max()
                             : world_pos->y / -world_speed.value()[1];
            if (t_y0 <= 0) t_y0 = std::numeric_limits<float>::max(); // 이미 지나쳤다면 무한대

            return t_y0;
        }

        return std::nullopt;
    }

    /**
     * @brief 현재 위치와 속도를 기반으로, y=0 혹은 y=ylim에 도달하는 위치를 예측합니다.
     * @param ylim y축의 끝 값
     * @return 예측된 3D 위치 (cm 단위).
     * @note get_world_positions_size() 으로 2개 이상의 위치가 있는지 확인해야 합니다.
     */
    [[nodiscard]] std::optional<cv::Point3f> get_arrive_pos(const float ylim = TABLE_HEIGHT) const {
        const auto world_pos = get_world_pos();
        const auto world_speed = get_world_speed();
        const auto t_arrive = get_arrive_time(world_pos, world_speed, ylim);

        if (!(world_pos.has_value() && world_speed.has_value() && t_arrive.has_value())) {
            Log::error(std::format(
                "[Predictor::get_arrive_pos] all optionals should to has_value(). world_pos={}, world_speed={}, t_arrive={}",
                world_pos.has_value() ? "exist" : "nullopt", world_speed.has_value() ? "exist" : "nullopt",
                t_arrive.has_value() ? "exist" : "nullopt"));
            return std::nullopt;
        }

        if (PREDICT_MIN_TIME <= t_arrive.value() && t_arrive.value() <= PREDICT_MAX_TIME)
            return predict_world_pos(world_pos.value(), world_speed.value(), t_arrive.value());

        Log::error(std::format(
            "[Predictor::get_arrive_pos] invalid t_arrive: not satisfies {} <= t_arrive(={}) <= {} when speed: [{}, {}, {}]",
            PREDICT_MIN_TIME, t_arrive.value(), PREDICT_MAX_TIME, world_speed.value()[0], world_speed.value()[1], world_speed.value()[2]));
        return std::nullopt;
    }
};

#endif //PREDICTOR_H
