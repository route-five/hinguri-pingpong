//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <filesystem>
#include <opencv2/opencv.hpp>
#include "camera_type.hpp"
#include "../utils/constants.hpp"

namespace fs = std::filesystem;

// TODO: 최적화

class Calibrator {
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
    explicit Calibrator(
        const CameraType& camera_type,
        const cv::Size& image_size = {1280, 720},
        const cv::Size& chessboard_grid = {8, 6},
        const float chessboard_square_size = 2.50f, // cm
        const cv::TermCriteria& criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1)
    ) : camera_type_{camera_type},
        image_size_{image_size},
        chessboard_grid_{chessboard_grid},
        chessboard_square_size_{chessboard_square_size},
        criteria_{criteria} {
        cv::FileStorage fs(camera_type_.calibration_matrix_path(), cv::FileStorage::READ);

        if (!fs.isOpened()) { // 보정 데이터가 없는 경우
            create_object_corners();
            glob_chessboard_images();
        }
        else { // 이미 보정 데이터가 존재하는 경우
            cv::Size image_size_fs;
            fs["camera_matrix"] >> camera_matrix_;
            fs["dist_coeffs"] >> dist_coeffs_;
            fs["rvecs"] >> rvecs_;
            fs["tvecs"] >> tvecs_;
            fs["image_size"] >> image_size_fs;
            fs["map1"] >> map1_;
            fs["map2"] >> map2_;

            assert(image_size_fs == image_size_ && "저장된 이미지 크기가 예상과 다릅니다.");
            assert(!camera_matrix_.empty() && "카메라 행렬이 비어 있습니다.");
            assert(!dist_coeffs_.empty() && "왜곡 계수가 비어 있습니다.");
            assert(!rvecs_.empty() && "회전 벡터가 비어 있습니다.");
            assert(!tvecs_.empty() && "이동 벡터가 비어 있습니다.");
            // map1_과 map2_는 calibrate(init_distort=false) 한 경우 비어있을 수 있음
        }
        fs.release();
    }

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
     * @param roi 유효 영역으로 이미지 크롭 (nullptr이면 전체 이미지 사용)
     * @param alpha 0에 가까울수록, 최대한 검정 테두리 없이 꽉 차나 시야가 좁아짐
     * @param save_data 보정 데이터를 저장할지 여부
     * @param init_undistort 왜곡 제거 맵을 초기화할지 여부
     */
    void calibrate(
        const std::vector<std::vector<cv::Point2f>>& image_points,
        const std::vector<std::vector<cv::Point3f>>& object_points,
        cv::Rect* roi = nullptr,
        const double alpha = 0.5,
        const bool save_data = true,
        const bool init_undistort = true
    ) {
        const double rms = cv::calibrateCamera(
            object_points, image_points, image_size_,
            camera_matrix_, dist_coeffs_, rvecs_, tvecs_
        );

        assert(rms < 0.4 && "카메라 보정 RMS 오차가 너무 높습니다. (rms >= 0.4)");

        if (init_undistort) {
            // TODO: new_camera_matrix를 앞으로의 연산으로 사용하나? = camera_matrix를 덮어 씌우는게 맞나?
            const cv::Mat new_camera_matrix = cv::getOptimalNewCameraMatrix(
                camera_matrix_, dist_coeffs_, image_size_,
                alpha, image_size_, roi
            );

            cv::initUndistortRectifyMap(
                camera_matrix_, dist_coeffs_,
                cv::Mat(), new_camera_matrix,
                image_size_, CV_32FC1, map1_, map2_
            );
        }

        if (save_data) {
            cv::FileStorage fs(camera_type_.calibration_matrix_path(), cv::FileStorage::WRITE);
            fs << "camera_matrix" << camera_matrix_;
            fs << "dist_coeffs" << dist_coeffs_;
            fs << "rvecs" << rvecs_;
            fs << "tvecs" << tvecs_;
            fs << "image_size" << image_size_;
            fs << "rms" << rms;
            if (init_undistort) {
                fs << "map1" << map1_;
                fs << "map2" << map2_;
            }
            fs.release();
        }
    }

    /**
     * @brief 왜곡 제거
     * @param input 입력 이미지 (왜곡된 이미지)
     * @param output 출력 이미지 (왜곡 제거된 이미지)
     * @param roi 유효 영역으로 output을 crop함 - nullptr이면 crop하지 않고 전체 이미지 사용
     */
    void undistort(const cv::Mat& input, cv::Mat& output, const cv::Rect* roi = nullptr) {
        if (map1_.empty() || map2_.empty()) { // 왜곡 제거 맵이 비어있다면 불러오기
            cv::FileStorage fs(camera_type_.calibration_matrix_path(), cv::FileStorage::READ);
            if (fs.isOpened()) {
                fs["map1"] >> map1_;
                fs["map2"] >> map2_;
            }
            fs.release();
        }

        assert(input.size() == image_size_ && "입력 이미지 크기가 예상과 다릅니다.");
        assert(!map1_.empty() && !map2_.empty() && "왜곡 제거 맵이 초기화되지 않았습니다. calibrate(init_undistort = true)를 먼저 호출하세요.");

        cv::remap(input, output, map1_, map2_, cv::INTER_LINEAR);
        if (roi != nullptr) {
            output = output(*roi);
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
        std::vector<cv::Point2f> camera_pts;

        cv::FileStorage fs_world(PATH_WORLD_POINTS, cv::FileStorage::READ);
        fs_world["points"] >> world_pts;
        fs_world.release();

        cv::FileStorage fs_camera(camera_type_.camera_points_path(), cv::FileStorage::READ);
        fs_camera["points"] >> camera_pts;
        fs_camera.release();

        assert(!world_pts.empty() && "월드 좌표계의 3D 점이 비어 있습니다. 먼저 탁구대의 3D 점을 정의하세요.");
        assert(!camera_pts.empty() && "카메라 이미지의 2D 점이 비어 있습니다. 먼저 카메라 이미지에서 2D 점을 검출하세요.");

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
            cv::FileStorage fs(camera_type_.projection_matrix_path(), cv::FileStorage::WRITE);
            fs << "R" << R;
            fs << "t" << t;
            fs.release();
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
        projection_matrix = camera_matrix_ * Rt; // P = K * [R | t]

        if (save_data) {
            cv::FileStorage fs(camera_type_.projection_matrix_path(), cv::FileStorage::WRITE);
            fs << "R" << R;
            fs << "t" << t;
            fs << "projection_matrix" << projection_matrix;
            fs.release();
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

        cv::FileStorage fs(camera_type_.projection_matrix_path(), cv::FileStorage::READ);
        fs["R"] >> R;
        fs["t"] >> t;
        fs.release();

        assert(!R.empty() && "카메라의 회전 행렬(R)이 비어 있습니다. 먼저 compute_camera_pose(save_data=true)를 호출하세요.");
        assert(!t.empty() && "카메라의 이동 벡터(t)가 비어 있습니다. 먼저 compute_camera_pose(save_data=true)를 호출하세요.");

        cv::Mat Rt;
        cv::hconcat(R, t, Rt); // [R | t]
        projection_matrix = camera_matrix_ * Rt; // P = K * [R | t]

        if (save_data) {
            cv::FileStorage fs_p(camera_type_.projection_matrix_path(), cv::FileStorage::WRITE);
            fs_p << "R" << R;
            fs_p << "t" << t;
            fs_p << "projection_matrix" << projection_matrix;
            fs_p.release();
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

    [[nodiscard]]

    const std::vector<cv::Point3f>& get_object_corners() const {
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

#endif //CALIBRATOR_H
