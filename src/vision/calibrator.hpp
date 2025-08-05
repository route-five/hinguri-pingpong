//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <filesystem>
#include <opencv2/opencv.hpp>

#include "camera.hpp"
#include "camera_type.hpp"
#include "../utils/constants.hpp"

namespace fs = std::filesystem;

namespace Calibrator {
    class Single {
    private:
        const CameraType camera_type_;
        const cv::Size chessboard_grid_;
        const cv::Size image_size_;
        const float chessboard_square_size_;
        const cv::TermCriteria criteria_;

        std::vector<cv::Point3f> object_corners_;
        std::vector<std::string> chessboard_image_paths_;
        cv::Mat camera_matrix_, dist_coeffs_, rvecs_, tvecs_;
        cv::Mat map1_, map2_;
        cv::Rect roi_;

        void create_object_corners() {
            object_corners_.clear();
            for (int r = 0; r < chessboard_grid_.height; ++r) {
                for (int c = 0; c < chessboard_grid_.width; ++c) {
                    object_corners_.emplace_back(c * chessboard_square_size_, r * chessboard_square_size_, 0.0f);
                }
            }
        }

        void glob_chessboard_images() {
            for (const auto& entry : fs::directory_iterator(camera_type_.calibration_image_dir())) {
                const auto& path = entry.path();
                if (path.extension() == ".png") {
                    chessboard_image_paths_.push_back(path.string());
                }
            }

            std::ranges::sort(chessboard_image_paths_);
        }

    public:
        explicit Single(
            const Camera& camera,
            const cv::Size& chessboard_grid = {8, 6},
            const float chessboard_square_size = 2.50f, // cm
            const cv::TermCriteria& criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1)
        ) : Single(
            camera.get_camera_type(),
            camera.get_frame_size(),
            chessboard_grid,
            chessboard_square_size,
            criteria
        ) {
        }

        explicit Single(
            const CameraType& camera_type,
            const cv::Size& chessboard_grid = {8, 6},
            const float chessboard_square_size = 2.50f, // cm
            const cv::TermCriteria& criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1)
        ) : Single(
            camera_type,
            camera_type.resolution(),
            chessboard_grid,
            chessboard_square_size,
            criteria
        ) {
        }

        explicit Single(
            const CameraType& camera_type,
            const cv::Size& image_size,
            const cv::Size& chessboard_grid = {8, 6},
            const float chessboard_square_size = 2.50f, // cm
            const cv::TermCriteria& criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1)
        ) : camera_type_{camera_type},
            image_size_{image_size},
            chessboard_grid_{chessboard_grid},
            chessboard_square_size_{chessboard_square_size},
            criteria_{criteria} {
            cv::Size image_size_fs;
            const bool success = camera_type_.read_calibration_matrix(
                &camera_matrix_, &dist_coeffs_, &rvecs_, &tvecs_,
                &image_size_fs, &map1_, &map2_, &roi_
            );

            if (success) {
                assert(image_size_fs == image_size_ && "저장된 이미지 크기가 예상과 다릅니다.");
            }
            else { // 보정 데이터가 없는 경우
                create_object_corners();
                glob_chessboard_images();
            }
        }

        ~Single() = default;

        /**
         * @brief 체스보드 코너 검출
         * @param image_points 체스보드 이미지에서 검출된 2D 이미지 좌표들
         * @param object_points 체스보드의 3D 월드 좌표들
         * @param show_debug_image 디버그 이미지 표시 여부
         */
        void find_chessboard_corners(
            std::vector<std::vector<cv::Point2f>>& image_points,
            std::vector<std::vector<cv::Point3f>>& object_points,
            const bool show_debug_image = false
        ) const {
            assert(
                !chessboard_image_paths_.empty() &&
                "카메라 보정을 위한 체스보드 이미지가 없습니다. 실제로 없거나, 생성자에서 이미 보정 데이터를 발견하여 체스보드 이미지 경로를 glob하지 않았을 수 있습니다.");

            for (const auto& image_path : chessboard_image_paths_) {
                cv::Mat frame = cv::imread(image_path), gray_frame;
                cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

                assert(frame.size() == image_size_ && "체스보드 이미지 크기가 예상과 다릅니다.");

                std::vector<cv::Point2f> corners;
                if (cv::findChessboardCorners(gray_frame, chessboard_grid_, corners)) {
                    cv::cornerSubPix(gray_frame, corners, {11, 11}, {-1, -1}, criteria_);
                    image_points.push_back(corners);
                    object_points.push_back(object_corners_);

                    if (show_debug_image) {
                        cv::drawChessboardCorners(frame, chessboard_grid_, corners, true);
                        cv::imshow("Find Chessboard Corners Debug", frame);
                        cv::waitKey(500);
                    }
                }
            }
        }

        /**
         * @brief 카메라 보정
         * @param image_points 체커보드 이미지에서 검출된 2D 이미지 좌표들
         * @param object_points 체커보드의 3D 월드 좌표들
         * @param roi 유효 영역으로 이미지 크롭
         * @param alpha 0에 가까울수록, 최대한 검정 테두리 없이 꽉 차나 시야가 좁아짐
         * @param save_data 보정 데이터를 저장할지 여부
         */
        void calibrate(
            const std::vector<std::vector<cv::Point2f>>& image_points,
            const std::vector<std::vector<cv::Point3f>>& object_points,
            cv::Rect& roi,
            const double alpha = 0.5,
            const bool save_data = true
        ) {
            const double rms = cv::calibrateCamera(
                object_points, image_points, image_size_,
                camera_matrix_, dist_coeffs_, rvecs_, tvecs_
            );

            assert(rms < 0.4 && "카메라 보정 RMS 오차가 너무 높습니다. (rms >= 0.4)");

            const cv::Mat new_camera_matrix = cv::getOptimalNewCameraMatrix(
                camera_matrix_, dist_coeffs_, image_size_,
                alpha, image_size_, &roi
            );

            cv::initUndistortRectifyMap(
                camera_matrix_, dist_coeffs_,
                cv::Mat(), new_camera_matrix,
                image_size_, CV_32FC1, map1_, map2_
            );

            if (save_data) {
                const bool success = camera_type_.write_calibration_matrix(
                    &camera_matrix_, &dist_coeffs_,
                    &rvecs_, &tvecs_, &image_size_, &rms, &map1_, &map2_, &roi
                );
                assert(success && "카메라 보정 데이터를 저장하는 데 실패했습니다.");
            }
        }

        /**
         * @brief 왜곡 제거
         * @param input 입력 이미지 (왜곡된 이미지)
         * @param output 출력 이미지 (왜곡 제거된 이미지)
         * @param crop_by_roi 유효 영역으로 output을 crop함 - nullptr이면 crop하지 않고 전체 이미지 사용
         */
        void undistort(const cv::Mat& input, cv::Mat& output, const bool crop_by_roi = true) {
            if (map1_.empty() || map2_.empty()) { // 왜곡 제거 맵이 비어있다면 불러오기
                camera_type_.read_calibration_matrix(nullptr, nullptr, nullptr, nullptr, nullptr, &map1_, &map2_);
            }

            assert(input.size() == image_size_ && "입력 이미지 크기가 예상과 다릅니다.");

            cv::remap(input, output, map1_, map2_, cv::INTER_LINEAR);
            if (crop_by_roi) {
                output = output(roi_);
            }
        }

        /**
         * @brief RANSAC 기반으로 카메라의 외부 파라미터(R, T)를 outlier가 있어도 안정적으로 추정합니다.
         * @param R (출력) 월드 → 카메라 회전을 나타내는 3×3 행렬
         * @param t (출력) 월드 → 카메라 변위를 나타내는 3×1 벡터
         * @param save_data 보정 데이터를 저장할지 여부
         */
        void compute_camera_pose(
            cv::Mat& R,
            cv::Mat& t,
            const bool save_data = true
        ) const {
            std::vector<cv::Point3f> world_pts;
            cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
            fs_world["points"] >> world_pts;
            fs_world.release();
            assert(!world_pts.empty() && "월드 좌표계의 3D 점이 비어 있습니다. 먼저 탁구대의 3D 점을 정의하세요.");

            std::vector<cv::Point2f> camera_pts;
            camera_type_.read_camera_points(camera_pts);

            cv::Mat rvec, tvec, inliers;
            cv::solvePnPRansac(
                world_pts,
                camera_pts,
                camera_matrix_,
                dist_coeffs_,
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

            if (save_data) {
                const bool success = camera_type_.write_projection_matrix(nullptr, &R, &t);
                assert(success && "카메라의 외부 파라미터(R, T)를 저장하는 데 실패했습니다.");
            }
        }

        /**
         * @brief 카메라의 projection 행렬을 계산합니다.
         * @param R 월드 → 카메라 회전을 나타내는 3×3 행렬
         * @param t 월드 → 카메라 변위를 나타내는 3×1 벡터
         * @param projection_matrix (출력) 3×4 projection 행렬 (P = K * [R | t])
         * @param save_data 보정 데이터를 저장할지 여부
         */
        void compute_projection_matrix(
            const cv::Mat& R,
            const cv::Mat& t,
            cv::Mat& projection_matrix,
            const bool save_data = true
        ) const {
            cv::Mat Rt;
            cv::hconcat(R, t, Rt); // [R | t]
            // cv::gemm(camera_matrix_, Rt, 1.0, cv::Mat(), 0.0, projection_matrix); // P = K * [R | t]
            projection_matrix = camera_matrix_ * Rt; // P = K * [R | t]

            if (save_data) {
                const bool success = camera_type_.write_projection_matrix(&projection_matrix, &R, &t);
                assert(success && "카메라의 projection 행렬(P)을 저장하는 데 실패했습니다.");
            }
        }

        /**
         * @brief 카메라의 projection 행렬을 계산합니다.
         * @param projection_matrix (출력) 3×4 projection 행렬 (P = K * [R | t])
         * @param save_data 보정 데이터를 저장할지 여부
         */
        void compute_projection_matrix(
            cv::Mat& projection_matrix,
            const bool save_data = true
        ) const {
            cv::Mat R, t;
            const bool success = camera_type_.read_projection_matrix(nullptr, &R, &t);
            assert(success && "카메라의 projection 행렬(P)을 읽는 데 실패했습니다.");

            assert(!R.empty() && "카메라의 회전 행렬(R)이 비어 있습니다. 먼저 compute_camera_pose(save_data=true)를 호출하세요.");
            assert(!t.empty() && "카메라의 이동 벡터(t)가 비어 있습니다. 먼저 compute_camera_pose(save_data=true)를 호출하세요.");

            cv::Mat Rt;
            cv::hconcat(R, t, Rt); // [R | t]
            // cv::gemm(camera_matrix_, Rt, 1.0, cv::Mat(), 0.0, projection_matrix); // P = K * [R | t]
            projection_matrix = camera_matrix_ * Rt; // P = K * [R | t]

            if (save_data) {
                const bool success2 = camera_type_.write_projection_matrix(&projection_matrix, &R, &t);
                assert(success2 && "카메라의 projection 행렬(P)을 저장하는 데 실패했습니다.");
            }
        }

        [[nodiscard]] const CameraType& get_camera_type() const {
            return camera_type_;
        }

        [[nodiscard]] const cv::Size& get_image_size() const {
            return image_size_;
        }

        [[nodiscard]] const cv::Size& get_chessboard_grid() const {
            return chessboard_grid_;
        }

        [[nodiscard]] float get_chessboard_square_size() const {
            return chessboard_square_size_;
        }

        [[nodiscard]] const std::vector<cv::Point3f>& get_object_corners() const {
            return object_corners_;
        }

        [[nodiscard]] const std::vector<std::string>& get_chessboard_image_paths() const {
            return chessboard_image_paths_;
        }

        [[nodiscard]] const cv::Mat& get_camera_matrix() const {
            return camera_matrix_;
        }

        [[nodiscard]] const cv::Mat& get_dist_coeffs() const {
            return dist_coeffs_;
        }

        [[nodiscard]] const cv::Mat& get_map1() const {
            return map1_;
        }

        [[nodiscard]] const cv::Mat& get_map2() const {
            return map2_;
        }
    };

    class Stereo {
    private:
        const Single calibrator_left;
        const Single calibrator_right;
        const StereoCameraType stereo_camera_type_;
        const cv::TermCriteria criteria_{cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1};

        /**
         * <code>R</code>: 첫 번째 카메라 좌표계에서 두 번째 카메라 좌표계로 회전시키는 3×3 행렬 <br>
         * <code>T</code>: 첫 번째 카메라에서 두 번째 카메라로의 위치 이동시키는 3×1 벡터 <br>
         * <code>E</code>: 카메라 내정(intrinsic)이 보정된 상태에서의 에피폴라 기하 정보를 담고 있는 행렬 <br>
         * <code>F</code>: 실제 픽셀 단위로 에피폴라 기하 관계를 표현하는 3×3 행렬
         */
        cv::Mat R, T, E, F;

        /**
         * <code>R1</code>: 첫 번째 카메라의 정렬 회전 행렬 <br>
         * <code>R2</code>: 두 번째 카메라의 정렬 회전 행렬 <br>
         * <code>P1</code>: 첫 번째 카메라의 정렬 투영 행렬 <br>
         * <code>P2</code>: 두 번째 카메라의 정렬 투영 행렬 <br>
         * <code>Q</code>: 재투영 행렬
         */
        cv::Mat R1, R2, P1, P2, Q;

        /**
         * <code>map1x</code>, <code>map1y</code>: 첫 번째 카메라의 왜곡 제거 맵 <br>
         * <code>map2x</code>, <code>map2y</code>: 두 번째 카메라의 왜곡 제거 맵 <br>
         */
        cv::Mat map1x, map1y, map2x, map2y;

        void load() {
            stereo_camera_type_.read_calibration_matrix(&R, &T, &E, &F, &map1x, &map1y, &map2x, &map2y);
            stereo_camera_type_.read_projection_matrix(&R1, &R2, &P1, &P2, &Q);
        }

    public:
        Stereo(const Camera& left, const Camera& right)
            : calibrator_left{left}, calibrator_right{right}, stereo_camera_type_{left.get_camera_type(), right.get_camera_type()} {
            assert("[Calibrator::Stereo::Stereo] 왼쪽과 오른쪽 카메라의 해상도가 동일해야 합니다." && left.get_frame_size() == right.get_frame_size());
            load();
        }

        Stereo(const CameraType& left_type, const CameraType& right_type)
            : calibrator_left{left_type}, calibrator_right{right_type}, stereo_camera_type_{left_type, right_type} {
            assert("[Calibrator::Stereo::Stereo] 왼쪽과 오른쪽 카메라의 해상도가 동일해야 합니다." && left_type.resolution() == right_type.resolution());
            load();
        }

        Stereo(const Single& left, const Single& right)
            : calibrator_left{left}, calibrator_right{right}, stereo_camera_type_{left.get_camera_type(), right.get_camera_type()} {
            assert("[Calibrator::Stereo::Stereo] 왼쪽과 오른쪽 카메라의 해상도가 동일해야 합니다." && left.get_image_size() == right.get_image_size());
            load();
        }

        [[nodiscard]] const Single& get_left_calibrator() const {
            return calibrator_left;
        }

        [[nodiscard]] const Single& get_right_calibrator() const {
            return calibrator_right;
        }

        [[nodiscard]] const CameraType& get_left_camera_type() const {
            return calibrator_left.get_camera_type();
        }

        [[nodiscard]] const CameraType& get_right_camera_type() const {
            return calibrator_right.get_camera_type();
        }

        [[nodiscard]] const StereoCameraType& get_camera_type() const {
            return stereo_camera_type_;
        }

        [[nodiscard]] const cv::Size& get_image_size() const {
            return calibrator_left.get_image_size();
        }

        /**
         * @brief 두 카메라를 이용한 스테레오 카메라 보정
         * @param image_points1 첫 번째 카메라의 체커보드 2D 이미지 좌표들
         * @param image_points2 두 번째 카메라의 체커보드 2D 이미지 좌표들
         * @param object_points 체커보드의 3D 월드 좌표들
         */
        void calibrate(
            const std::vector<std::vector<cv::Point2f>>& image_points1,
            const std::vector<std::vector<cv::Point2f>>& image_points2,
            const std::vector<std::vector<cv::Point3f>>& object_points
        ) const {
            assert(image_points1.size() == image_points2.size());
            assert(image_points1.size() == object_points.size());

            const double rms = cv::stereoCalibrate(
                object_points,
                image_points1,
                image_points2,
                calibrator_left.get_camera_matrix(),
                calibrator_left.get_dist_coeffs(),
                calibrator_right.get_camera_matrix(),
                calibrator_right.get_dist_coeffs(),
                get_image_size(),
                R,
                T,
                E,
                F,
                cv::CALIB_FIX_INTRINSIC,
                criteria_
            );

            assert(rms < 1.0 && "스테레오 보정 RMS 오차가 너무 큽니다.");

            const bool success = stereo_camera_type_.write_calibration_matrix(&R, &T, &E, &F);
            assert(success && "스테레오 카메라 보정 데이터를 저장하는 데 실패했습니다.");
        }

        /**
         * @brief 스테레오 정렬 (rectification) 수행 및 결과 저장
         */
        void rectify() const {
            assert((!R.empty() && !T.empty()) && "R, T 행렬이 비어있습니다. 스테레오 정렬을 수행하기 전에 먼저 stereo_calibrate()를 호출해야 합니다.");

            cv::stereoRectify(
                calibrator_left.get_camera_matrix(),
                calibrator_left.get_dist_coeffs(),
                calibrator_right.get_camera_matrix(),
                calibrator_right.get_dist_coeffs(),
                get_image_size(),
                R,
                T,
                R1,
                R2,
                P1,
                P2,
                Q,
                cv::CALIB_ZERO_DISPARITY,
                0,
                get_image_size()
            );

            const bool success = stereo_camera_type_.write_projection_matrix(&R1, &R2, &P1, &P2, &Q);
            assert(success && "스테레오 정렬 행렬(R1, R2, P1, P2, Q)을 저장하는 데 실패했습니다.");
        }

        /**
         * @brief 스테레오 정렬 맵을 초기화합니다.
         */
        void init_rectification_maps(
            cv::Mat& map1x,
            cv::Mat& map1y,
            cv::Mat& map2x,
            cv::Mat& map2y
        ) const {
            assert(
                (!R1.empty() && !R2.empty() && !P1.empty() && !P2.empty()) &&
                "R1, R2, P1, P2 행렬이 비어있습니다. 스테레오 정렬 맵을 초기화하기 전에 먼저 stereo_rectify()를 호출해야 합니다."
            );

            cv::initUndistortRectifyMap(
                calibrator_left.get_camera_matrix(),
                calibrator_left.get_dist_coeffs(),
                R1,
                P1,
                get_image_size(),
                CV_16SC2,
                map1x,
                map1y
            );

            cv::initUndistortRectifyMap(
                calibrator_right.get_camera_matrix(),
                calibrator_right.get_dist_coeffs(),
                R2,
                P2,
                get_image_size(),
                CV_16SC2,
                map2x,
                map2y
            );

            const bool success = stereo_camera_type_.write_calibration_matrix(&R, &T, &E, &F, &map1x, &map1y, &map2x, &map2y);
            assert(success && "스테레오 정렬 맵(map1x, map1y, map2x, map2y)을 저장하는 데 실패했습니다.");
        }

        /**
         * @brief 스테레오 정렬 remap을 수행합니다.
         * @param frame1 좌측 원본 이미지
         * @param frame2 우측 원본 이미지
         * @param rectified1 (출력) 좌측 정렬 이미지
         * @param rectified2 (출력) 우측 정렬 이미지
         */
        void rectify_image_pair(
            const cv::Mat& frame1,
            const cv::Mat& frame2,
            cv::Mat& rectified1,
            cv::Mat& rectified2
        ) const {
            assert((!map1x.empty() && !map1y.empty() && !map2x.empty() && !map2y.empty()) &&
                "왜곡 제거 맵이 비어있습니다. 스테레오 정렬 맵을 초기화하기 전에 먼저 init_stereo_rectification_maps()를 호출해야 합니다."
            );

            cv::remap(frame1, rectified1, map1x, map1y, cv::INTER_LINEAR);
            cv::remap(frame2, rectified2, map2x, map2y, cv::INTER_LINEAR);
        }
    };
};

#endif //CALIBRATOR_H
