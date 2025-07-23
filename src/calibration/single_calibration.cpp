//
// Created by Hyunseung Ryu on 2025. 7. 22..
//

#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../utils/camera_type.hpp"
#define IMSHOW

namespace fs = std::filesystem;

std::vector<std::string> glob_chessboard_images(const std::string& folder) {
    std::vector<std::string> result;

    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        const auto& path = entry.path();
        if (path.extension() == ".png") {
            result.push_back(path.string());
        }
    }

    std::ranges::sort(result);
    return result;
}

std::vector<cv::Point3f> create_object_corners(const cv::Size& chessboard_grid, const float square_size) {
    std::vector<cv::Point3f> object_corners(chessboard_grid.width * chessboard_grid.height);
    for (int r = 0; r < chessboard_grid.height; ++r) {
        for (int c = 0; c < chessboard_grid.width; ++c) {
            object_corners[r * chessboard_grid.width + c] = cv::Point3f(
                c * square_size, r * square_size, 0.0f);
        }
    }

    return object_corners;
}

int main() {
    const CameraType camera_type = CameraType::LEFT;
    const cv::Size chessboard_grid(8, 6); // 실제론 9*7 사각형 격자였는데 안쪽 격자점만 탐지하기 때문에 8*6 격자점
    const cv::Size image_size(1280, 720);
    constexpr float chessboard_square_size = 25.0f; // mm
    const auto criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1);

    auto object_corners = create_object_corners(chessboard_grid, chessboard_square_size);
    const auto image_paths = glob_chessboard_images(camera_type.calibration_image_dir());
    assert(!image_paths.empty() && "No images found for calibration.");

    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<std::vector<cv::Point2f>> image_points;

    // find corners
    for (const auto& image_path : image_paths) {
#ifdef IMSHOW
        std::cout << std::endl << "Processing image: " << image_path << std::endl;
#endif

        cv::Mat frame = cv::imread(image_path), gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        assert(frame.size() == image_size && "Image size does not match the expected size.");

        std::vector<cv::Point2f> corners;
        if (cv::findChessboardCorners(gray, chessboard_grid, corners)) {
#ifdef IMSHOW
            std::cout << "Found Chessboard Corners!" << std::endl;
#endif

            cv::cornerSubPix(gray, corners, {11, 11}, {-1, -1}, criteria);
            cv::drawChessboardCorners(frame, chessboard_grid, corners, true);

            image_points.push_back(corners);
            object_points.push_back(object_corners);

#ifdef IMSHOW
            cv::imshow("Single Calibration", frame);
            // cv::waitKey(100);
            int key;
            do {
                key = cv::waitKey(0);
                if (key == 'q') return 0;
            }
            while (key != ' ');
#endif
        }
        else {
#ifdef IMSHOW
            std::cout << "Chessboard Corners not found" << std::endl;
#endif
        }
    }

    cv::destroyAllWindows();

    // calibrate
    cv::Mat camera_matrix; // 내부 파라미터 행렬
    cv::Mat dist_coeffs; // 렌즈 왜곡 계수
    cv::Mat rvecs; // 각 각도에서의 외부 파라미터 R
    cv::Mat tvecs; // 각 각도에서의 외부 파라미터 t
    double rms = cv::calibrateCamera(
        object_points, image_points, image_size,
        camera_matrix, dist_coeffs, rvecs, tvecs
    );

    std::cout << "\n================= Calibration Results =================\n";
    std::cout << "RMS Re-projection Error: " << rms << "\n\n";
    std::cout << "Camera Intrinsic Matrix:\n" << camera_matrix << "\n\n";
    std::cout << "Distortion Coefficients:\n" << dist_coeffs;
    std::cout << "\n=======================================================\n";

    // Save calibration results
    cv::FileStorage fs(camera_type.calibration_matrix_path(), cv::FileStorage::WRITE);
    fs << "camera_matrix" << camera_matrix;
    fs << "dist_coeffs" << dist_coeffs;
    fs << "rvecs" << rvecs;
    fs << "tvecs" << tvecs;
    fs << "image_size" << image_size;
    fs << "rms" << rms;
    fs.release();

    // undistort
    constexpr double alpha = 0.5; // 0에 가까울 수록, 최대한 왜곡을 제거하고 검정
    // 테두리 없이 꽉 차나 시야가 좁아짐

    for (const auto& image_path : image_paths) {
#ifdef IMSHOW
        std::cout << std::endl << "Processing image: " << image_path << std::endl;
#endif

        cv::Mat frame = cv::imread(image_path);

        cv::Rect roi; // 유효한 영역
        cv::Mat new_camera_matrix = cv::getOptimalNewCameraMatrix(
            camera_matrix, dist_coeffs, image_size,
            alpha, image_size, &roi
        );

        cv::Mat undistorted;
        cv::undistort(frame, undistorted, camera_matrix, dist_coeffs, new_camera_matrix);

        // cv::Mat cropped = undistorted(roi);  // 이렇게 잘라낼 수 있음
        cv::rectangle(undistorted, roi, COLOR_GREEN, 2);

#ifdef IMSHOW
        // ROI 커버리지 계산
        double roi_area = roi.width * roi.height;
        double image_area = undistorted.cols * undistorted.rows;
        double roi_percentage = (roi_area / image_area) * 100.0;
        std::cout << "ROI covers " << roi_percentage << "% of the full image."
            << std::endl;

        cv::imshow("Undistorted with ROI", undistorted);
        // cv::waitKey(100);
        int key;
        do {
            key = cv::waitKey(0);
            if (key == 'q') return 0;
        }
        while (key != ' ');
#endif
    }

    return 0;
}
