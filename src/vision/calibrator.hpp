//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <filesystem>
#include <opencv2/opencv.hpp>
#include "camera_type.hpp"

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
        for (int r = 0; r < chessboard_grid_.height; ++r) {
            for (int c = 0; c < chessboard_grid_.width; ++c) {
                object_corners_[r * chessboard_grid_.width + c] = cv::Point3f(
                    c * chessboard_square_size_, r * chessboard_square_size_, 0.0f);
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
        const cv::Size& chessboard_grid = {8, 6},
        const cv::Size& image_size = {1280, 720},
        const float chessboard_square_size = 25.0f,
        const cv::TermCriteria& criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1)
    ) : camera_type_{camera_type},
        chessboard_grid_{chessboard_grid},
        image_size_{image_size},
        chessboard_square_size_{chessboard_square_size},
        criteria_{criteria} {
        object_corners_.resize(chessboard_grid_.width * chessboard_grid_.height);
        create_object_corners();
        glob_chessboard_images();

        assert(!chessboard_image_paths_.empty() && "카메라 보정을 위한 체스보드 이미지가 없습니다.");
    }

    void find_chessboard_corners(
        std::vector<std::vector<cv::Point2f>>& image_points,
        std::vector<std::vector<cv::Point3f>>& object_points
    ) const {
        for (const auto& image_path : chessboard_image_paths_) {
            cv::Mat frame = cv::imread(image_path);
            assert(frame.size() == image_size_ && "체스보드 이미지 크기가 예상과 다릅니다.");
            assert(frame.channels() == 1 && "이미지가 grayscale (1채널)이 아닙니다.");

            std::vector<cv::Point2f> corners;
            if (cv::findChessboardCorners(frame, chessboard_grid_, corners)) {
                cv::cornerSubPix(frame, corners, {11, 11}, {-1, -1}, criteria_);
                image_points.push_back(corners);
                object_points.push_back(object_corners_);
            }
        }
    }

    /**
     * @param image_points
     * @param object_points
     * @param alpha 0에 가까울수록, 최대한 검정 테두리 없이 꽉 차나 시야가 좁아짐
     * @param init_undistort
     * @param save_data
     * @param roi
     */
    void calibrate(
        const std::vector<std::vector<cv::Point2f>>& image_points,
        const std::vector<std::vector<cv::Point3f>>& object_points,
        const double alpha = 0.5,
        const bool init_undistort = true,
        const bool save_data = true,
        cv::Rect* roi = nullptr
    ) const {
        const double rms = cv::calibrateCamera(
            object_points, image_points, image_size_,
            camera_matrix_, dist_coeffs_, rvecs_, tvecs_
        );

        assert(rms < 0.4 && std::format("카메라 보정 RMS 오차가 너무 높습니다. ({} >= 0.4)", rms).data());

        if (init_undistort) {
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
            fs.release();
        }
    }

    void undistort(const cv::Mat& input, cv::Mat& output, const cv::Rect* roi = nullptr) const {
        assert(input.size() == image_size_ && "입력 이미지 크기가 예상과 다릅니다.");
        assert(!map1_.empty() && !map2_.empty() && "왜곡 제거 맵이 초기화되지 않았습니다. calibrate(init_undistort = true)를 먼저 호출하세요.");

        cv::remap(input, output, map1_, map2_, cv::INTER_LINEAR);
        if (roi != nullptr) {
            output = output(*roi);
        }
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

    [[nodiscard]] const CameraType& get_camera_type() const {
        return camera_type_;
    }

    [[nodiscard]] const cv::Size& get_image_size() const {
        return image_size_;
    }

    [[nodiscard]] const cv::Mat& get_map1() const {
        return map1_;
    }

    [[nodiscard]] const cv::Mat& get_map2() const {
        return map2_;
    }
};

#endif //CALIBRATOR_H
