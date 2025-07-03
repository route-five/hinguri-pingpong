#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "constants.hpp"

cv::Vec3d pixel_to_camera_ray(const cv::Point2f& pixel_coord, const cv::Mat& camera_matrix) {
    const double fx = camera_matrix.at<double>(0, 0);
    const double fy = camera_matrix.at<double>(1, 1);
    const double cx = camera_matrix.at<double>(0, 2);
    const double cy = camera_matrix.at<double>(1, 2);

    const double x = (pixel_coord.x - cx) / fx;
    const double y = (pixel_coord.y - cy) / fy;

    const cv::Vec3d ray_direction(x, y, 1.0);
    return cv::normalize(ray_direction);
}

cv::Point3d intersect_ray_with_plane(const cv::Vec3d& ray_direction, const cv::Vec3d& camera_position, double target_z) {
    const double t = (target_z - camera_position[2]) / ray_direction[2];
    double x = camera_position[0] + t * ray_direction[0];
    double y = camera_position[1] + t * ray_direction[1];
    return {x, y, target_z};
}

cv::Vec3d camera_to_world(const cv::Vec3d& point_in_camera, const cv::Mat& rotation, const cv::Mat& translation) {
    const cv::Matx31d cam_point({point_in_camera[0], point_in_camera[1], point_in_camera[2]});
    const cv::Mat world_point = rotation.t() * (cam_point - translation);
    return cv::Vec3d(world_point);
}

std::vector<double> quadratic_fit(const std::vector<double>& xs, const std::vector<double>& ys) {
    double sum_x = 0, sum_x2 = 0, sum_x3 = 0, sum_x4 = 0;
    double sum_y = 0, sum_xy = 0, sum_x2y = 0;

    for (int i = 0; i < xs.size(); i++) {
        const double x = xs[i];
        const double y = ys[i];

        sum_x += x;
        sum_x2 += x * x;
        sum_x3 += x * x * x;
        sum_x4 += x * x * x * x;

        sum_y += y;
        sum_xy += x * y;
        sum_x2y += x * x * y;
    }

    const cv::Matx33d A({
        static_cast<double>(xs.size()), sum_x, sum_x2,
        sum_x, sum_x2, sum_x3,
        sum_x2, sum_x3, sum_x4
    });
    const cv::Matx31d B({sum_y, sum_xy, sum_x2y});

    cv::Mat coeffs;
    cv::solve(A, B, coeffs, cv::DECOMP_SVD);

    return {coeffs.at<double>(2), coeffs.at<double>(1), coeffs.at<double>(0)};
}

double evaluate_quadratic(const std::vector<double>& coeffs, const double x) {
    return coeffs[0]*x*x + coeffs[1]*x + coeffs[2];
}

void process_ball_trajectory(
    const std::vector<cv::Point2f>& detected_pixels,
    const cv::Mat& camera_matrix,
    const cv::Mat& rotation,
    const cv::Mat& translation,
    const double target_plane_z
) {
    std::vector<double> x_points, z_points;

    const cv::Vec3d camera_position(
        translation.at<double>(0),
        translation.at<double>(1),
        translation.at<double>(2)
    );

    for (const auto& pixel : detected_pixels) {
        cv::Vec3d ray_direction = pixel_to_camera_ray(pixel, camera_matrix);
        const cv::Point3d plane_intersection = intersect_ray_with_plane(ray_direction, camera_position, target_plane_z);
        cv::Vec3d world_point = camera_to_world(
            cv::Vec3d(plane_intersection.x, plane_intersection.y, plane_intersection.z),
            rotation,
            translation
        );

        x_points.push_back(world_point[0]);
        z_points.push_back(world_point[2]);
    }

    if (x_points.size() >= 3) {
        const std::vector<double> coeffs = quadratic_fit(x_points, z_points);
        std::cout << "회귀식: z = " << coeffs[0] << " * x^2 + " << coeffs[1] << " * x + " << coeffs[2] << std::endl;

        for (const double x_test : {1.0, 2.0, 3.0}) {
            const double z_test = evaluate_quadratic(coeffs, x_test);
            std::cout << "x=" << x_test << "일 때 예측 높이 z=" << z_test << std::endl;
        }
    } else {
        std::cout << "데이터 포인트 부족 (최소 3개 필요)" << std::endl;
    }
}

int main() {
    const std::vector<std::vector<cv::Point2f>> test_cases = {
        { {310, 200}, {315, 180}, {320, 160} },
        { {300, 210}, {305, 190}, {310, 170} },
        { {330, 190}, {335, 170}, {340, 150} },
        { {290, 220}, {295, 200}, {300, 180} }
    };

    for (int i = 0; i < test_cases.size(); ++i) {
        std::cout << "\n[Test Case " << i + 1 << "]" << std::endl;
        process_ball_trajectory(test_cases[i], CAMERA_MATRIX, CAMERA_ROTATION, CAMERA_TRANSLATION, TARGET_PLANE_Z);
    }

    return 0;
}