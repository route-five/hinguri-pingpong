//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#include <opencv2/opencv.hpp>
#include <vector>

#include "vision/camera_type.hpp"

// TODO: 모두 통합하기

int main() {
    // (1) Projection 행렬 로드
    cv::Mat proj_left, proj_right;

    cv::FileStorage fs_left(CameraType::LEFT.projection_matrix_path(), cv::FileStorage::READ);
    fs_left["projection_matrix"] >> proj_left;
    fs_left.release();

    cv::FileStorage fs_right(CameraType::RIGHT.projection_matrix_path(), cv::FileStorage::READ);
    fs_right["projection_matrix"] >> proj_right;
    fs_right.release();

    // (2) 3대의 카메라 타임라인 동기화
    // TODO: 카메라 동기화 로직 구현 필요 - 구현 거의 완료 at PONG#60

    // (3) 탁구공 센터 검출 (예: blob)
    // TODO: 공 검출 로직 구현 필요 - 구현 완료 at tracker.hpp
    cv::Point2f center_left{/* CAM1에서 탐지된 공 중심 좌표 */};
    cv::Point2f center_right{/* CAM2에서 탐지된 공 중심 좌표 */};

    // (4) 3D 위치 삼각측량
    cv::Point3f ball_pos = triangulate_ball_3d(proj_left, proj_right, center_left, center_right);
    std::cout << "Ball 3D pos (cm): ["
        << ball_pos.x << ", "
        << ball_pos.y << ", "
        << ball_pos.z << "]" << std::endl;

    // (5) Top 카메라와 비교해서 결과 정밀하게 비교
    // TODO: Top 카메라와 비교하는 로직 구현 필요

    // (6) Kalman filter 등의 후처리로 결과 보정
    // TODO: Kalman filter 적용 로직 구현 필요

    // (7) 탁구공 미래 궤적 예측
    // TODO: 미래 궤적 예측 로직 구현 필요 - 구현 거의 완료 at quadratic_regression.cpp

    // (8) 예상 도착 위치 및 각도 정보를 토대로 하드웨어에 전송할 인자 계산
    // TODO: 하드웨어 제어 인자 계산 로직 구현 필요
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

    // (9) 하드웨어에 제어 인자 + 명령 전송
    // TODO: 하드웨어 제어 명령 전송 로직 구현 필요

    return 0;
}
