//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <opencv2/opencv.hpp>
#include "camera_type.hpp"

class Predictor {
    // TODO: Predictor
    //  2. stereo calibration으로 3D 위치 추정하기까지 구현하기
    //  3. 궤적 회귀 - quadratic_regression.cpp 로부터 살짝 수정해서 가져오기
    //  4. Kalman filter로 보정

private:
    cv::Mat R_left, R_right;
    cv::Mat t_left, t_right;
    cv::Mat P_left, P_right;

    cv::Point2f pt_left, pt_right;

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
    }

    ~Predictor() = default;

    void set_point_left(const cv::Point2f& left) {
        pt_left = left;
    }

    void set_point_right(const cv::Point2f& right) {
        pt_right = right;
    }

    [[nodiscard]] const cv::Point2f& get_point_left() const {
        return pt_left;
    }

    [[nodiscard]] const cv::Point2f& get_point_right() const {
        return pt_right;
    }

    /**
     * @brief 두 카메라 projection 행렬을 사용해 2D 대응점으로부터 3D 위치를 계산합니다.
     * @return 계산된 3D 점(Pinhole world coordinates, 동차 좌표 정규화 완료).
     * @note OpenCV의 triangulatePoints()는 결과를 동차 좌표(4×1)로 반환하므로,
     *       w로 나눠서 실제 xyz 좌표를 반환합니다.
     */
    [[nodiscard]] cv::Point3f get_world_pos() const {
        const cv::Mat pts1(1, 1, CV_32FC2, cv::Scalar(pt_left.x, pt_left.y));
        const cv::Mat pts2(1, 1, CV_32FC2, cv::Scalar(pt_right.x, pt_right.y));

        cv::Mat pts4d;
        cv::triangulatePoints(P_left, P_right, pts1, pts2, pts4d);

        cv::Mat p = pts4d.col(0);
        p /= p.at<float>(3); // 동차 좌표 정규화

        return {p.at<float>(0), p.at<float>(1), p.at<float>(2)};
    }
};

#endif //PREDICTOR_H
