#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "constants.hpp"

cv::Vec3d pixel_to_camera_ray(const cv::Point2f& pixel_coord, const cv::Mat& camera_matrix) {
    double fx = camera_matrix.at<double>(0, 0);
    double fy = camera_matrix.at<double>(1, 1);
    double cx = camera_matrix.at<double>(0, 2);
    double cy = camera_matrix.at<double>(1, 2);

    double x = (pixel_coord.x - cx) / fx;
    double y = (pixel_coord.y - cy) / fy;

    cv::Vec3d ray_direction(x, y, 1.0);
    return cv::normalize(ray_direction);
}

cv::Point3d intersect_ray_with_plane(const cv::Vec3d& ray_direction, const cv::Vec3d& camera_position, double target_z) {
    double t = (target_z - camera_position[2]) / ray_direction[2];
    double x = camera_position[0] + t * ray_direction[0];
    double y = camera_position[1] + t * ray_direction[1];
    return cv::Point3d(x, y, target_z);
}

cv::Vec3d camera_to_world(const cv::Vec3d& point_in_camera, const cv::Mat& rotation, const cv::Mat& translation) {
    cv::Mat cam_point = (cv::Mat_<double>(3,1) << point_in_camera[0], point_in_camera[1], point_in_camera[2]);
    cv::Mat world_point = rotation.t() * (cam_point - translation);
    return cv::Vec3d(world_point);
}

std::vector<double> quadratic_fit(const std::vector<double>& xs, const std::vector<double>& ys) {
    int n = xs.size();
    double sum_x = 0, sum_x2 = 0, sum_x3 = 0, sum_x4 = 0;
    double sum_y = 0, sum_xy = 0, sum_x2y = 0;

    for (int i = 0; i < n; i++) {
        double x = xs[i];
        double y = ys[i];
        sum_x += x;
        sum_x2 += x * x;
        sum_x3 += x * x * x;
        sum_x4 += x * x * x * x;

        sum_y += y;
        sum_xy += x * y;
        sum_x2y += x * x * y;
    }

    cv::Mat A = (cv::Mat_<double>(3,3) << n, sum_x, sum_x2,
                                          sum_x, sum_x2, sum_x3,
                                          sum_x2, sum_x3, sum_x4);
    cv::Mat B = (cv::Mat_<double>(3,1) << sum_y, sum_xy, sum_x2y);

    cv::Mat coeffs;
    cv::solve(A, B, coeffs, cv::DECOMP_SVD);

    return {coeffs.at<double>(2), coeffs.at<double>(1), coeffs.at<double>(0)};
}

double evaluate_quadratic(const std::vector<double>& coeffs, double x) {
    return coeffs[0]*x*x + coeffs[1]*x + coeffs[2];
}

void process_ball_trajectory(
    const std::vector<cv::Point2f>& detected_pixels,
    const cv::Mat& camera_matrix,
    const cv::Mat& rotation,
    const cv::Mat& translation,
    double target_plane_z
) {
    std::vector<double> x_points, z_points;

    cv::Vec3d camera_position(
        translation.at<double>(0),
        translation.at<double>(1),
        translation.at<double>(2)
    );

    for (const auto& pixel : detected_pixels) {
        cv::Vec3d ray_direction = pixel_to_camera_ray(pixel, camera_matrix);
        cv::Point3d plane_intersection = intersect_ray_with_plane(ray_direction, camera_position, target_plane_z);
        cv::Vec3d world_point = camera_to_world(
            cv::Vec3d(plane_intersection.x, plane_intersection.y, plane_intersection.z),
            rotation,
            translation
        );

        x_points.push_back(world_point[0]);
        z_points.push_back(world_point[2]);
    }

    if (x_points.size() >= 3) {
        std::vector<double> coeffs = quadratic_fit(x_points, z_points);
        std::cout << "회귀식: z = " << coeffs[0] << " * x^2 + " << coeffs[1] << " * x + " << coeffs[2] << std::endl;

        double x_test = 2.0;
        double z_test = evaluate_quadratic(coeffs, x_test);
        std::cout << "x=" << x_test << "일 때 예측 높이 z=" << z_test << std::endl;
    } else {
        std::cout << "데이터 포인트 부족 (최소 3개 필요)" << std::endl;
    }
}

int main() {
    std::vector<cv::Point2f> detected_pixels = {
        cv::Point2f(310,200),
        cv::Point2f(315,180),
        cv::Point2f(320,160)
    };

    process_ball_trajectory(detected_pixels, CAMERA_MATRIX, CAMERA_ROTATION, CAMERA_TRANSLATION, TARGET_PLANE_Z);

    return 0;
}
