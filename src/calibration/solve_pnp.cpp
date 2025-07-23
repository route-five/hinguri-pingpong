//
// Created by Hyunseung Ryu on 2025. 7. 23..
//

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * @brief RANSAC 기반으로 카메라의 외부 파라미터(R, T)를 robust하게 추정합니다.
 * @param world_pts 월드 좌표계(예: 탁구대)에 정의된 3D 점들의 벡터.
 * @param img_pts 해당 3D 점들에 대응하는 이미지 상의 2D 픽셀 좌표 벡터.
 * @param camera_matrix 해당 카메라의 내부 파라미터 행렬 (3×3).
 * @param dist_coeffs 해당 카메라의 왜곡 계수 벡터.
 * @param R (출력) 월드→카메라 회전을 나타내는 3×3 행렬.
 * @param t (출력) 월드→카메라 변위를 나타내는 3×1 벡터.
 *
 * @note 내부적으로 solvePnPRansac()를 사용하며, outlier를 제거하면서 R과 t를 추정합니다.
 */
void compute_camera_pose_ransac(
    const std::vector<cv::Point3f>& world_pts,
    const std::vector<cv::Point2f>& img_pts,
    const cv::Mat& camera_matrix,
    const cv::Mat& dist_coeffs,
    cv::Mat& R,
    cv::Mat& t
) {
    cv::Mat rvec, tvec, inliers;
    cv::solvePnPRansac(
        world_pts,
        img_pts,
        camera_matrix,
        dist_coeffs,
        rvec,
        tvec,
        false, // 매개변수 추정 초기값 사용 안 함
        100, // 최대 반복 횟수
        8.0, // reprojection 오차 threshold (픽셀 단위)
        0.99, // 성공 확률
        inliers, // inlier 인덱스 저장 벡터
        cv::SOLVEPNP_ITERATIVE
    );
    cv::Rodrigues(rvec, R); // 회전 벡터 → 회전 행렬
    t = tvec.clone();
}

/**
 * @brief 두 카메라 projection 행렬을 사용해 2D 대응점으로부터 3D 위치를 계산합니다.
 * @param proj_mat1 첫 번째 카메라의 3×4 projection matrix (K*[R|t]).
 * @param proj_mat2 두 번째 카메라의 3×4 projection matrix.
 * @param pt1 첫 번째 카메라 이미지에서의 2D 점 (픽셀 좌표).
 * @param pt2 두 번째 카메라 이미지에서의 2D 점.
 * @return 계산된 3D 점(Pinhole world coordinates, 동차 좌표 정규화 완료).
 *
 * @note OpenCV의 triangulatePoints()는 결과를 동차 좌표(4×1)로 반환하므로,
 *       w로 나눠서 실제 xyz 좌표를 반환합니다.
 */
cv::Point3f triangulate_ball_3d(
    const cv::Mat& proj_mat1,
    const cv::Mat& proj_mat2,
    const cv::Point2f& pt1,
    const cv::Point2f& pt2
) {
    cv::Mat pts1(1, 1, CV_32FC2, cv::Scalar(pt1.x, pt1.y));
    cv::Mat pts2(1, 1, CV_32FC2, cv::Scalar(pt2.x, pt2.y));
    cv::Mat pts4d;
    cv::triangulatePoints(proj_mat1, proj_mat2, pts1, pts2, pts4d);
    cv::Mat p = pts4d.col(0);
    p /= p.at<float>(3); // 동차 좌표 정규화

    return {p.at<float>(0), p.at<float>(1), p.at<float>(2)};
}

int main() {
    // (1) 카메라 내부 파라미터 로딩
    cv::Mat k_left, d_left, k_right, d_right;

    cv::FileStorage fs_left("left.yml", cv::FileStorage::READ);
    fs_left["camera_matrix"] >> k_left;
    fs_left["dist_coeffs"] >> d_left;
    fs_left.release();

    cv::FileStorage fs_right("right.yml", cv::FileStorage::READ);
    fs_right["camera_matrix"] >> k_right;
    fs_right["dist_coeffs"] >> d_right;
    fs_right.release();

    // TODO: 탁구대 위에 절연 테이프로 포인트 추가하기
    // (2) 월드 좌표계 기준 3D 점 (탁구대 네 모서리)
    std::vector<cv::Point3f> world_pts = {
        {0.f, 0.f, 0.f},
        {1525.f, 0.f, 0.f},
        {0.f, 2740.f, 0.f},
        {1525.f, 2740.f, 0.f}
    };

    // TODO: 이미지에서 클릭해서 얻는 코드 만들기
    // (3) 각 카메라에서 대응되는 2D 이미지 좌표 (수집 필요)
    std::vector<cv::Point2f> img_pts_left; // CAM1 대응점
    std::vector<cv::Point2f> img_pts_right; // CAM2 대응점

    // (4) R, T 계산
    cv::Mat r_left, t_left, r_right, t_right;
    compute_camera_pose_ransac(world_pts, img_pts_left, k_left, d_left, r_left, t_left);
    compute_camera_pose_ransac(world_pts, img_pts_right, k_right, d_right, r_right, t_right);

    // (5) Projection 행렬 구성: P = K * [R | t]
    cv::Mat proj_left, proj_right;
    cv::hconcat(r_left, t_left, proj_left);
    proj_left = k_left * proj_left;
    cv::hconcat(r_right, t_right, proj_right);
    proj_right = k_right * proj_right;

    // (6) 탁구공 센터 검출 (예: blob)
    cv::Point2f center_left{/* CAM1에서 탐지된 공 중심 좌표 */};
    cv::Point2f center_right{/* CAM2에서 탐지된 공 중심 좌표 */};

    // (7) 3D 위치 삼각측량
    cv::Point3f ball_pos = triangulate_ball_3d(proj_left, proj_right, center_left, center_right);
    std::cout << "Ball 3D pos (mm): ["
        << ball_pos.x << ", "
        << ball_pos.y << ", "
        << ball_pos.z << "]" << std::endl;

    return 0;
}
