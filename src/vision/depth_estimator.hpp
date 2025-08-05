//
// Created by Hyunseung Ryu on 2025. 8. 5..
//

#ifndef DEPTH_ESTIMATOR_HPP
#define DEPTH_ESTIMATOR_HPP
#include <opencv2/calib3d.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>

#include "camera.hpp"

class DepthEsmimator {
private:
    cv::Size image_size;
    cv::UMat map1x, map1y, map2x, map2y;
    cv::UMat Q, disparity;
    cv::UMat rectified_left, rectified_right;
    cv::UMat gray_left, gray_right;

    cv::Ptr<cv::StereoSGBM> stereo_matcher;
    const int min_disparity = 0;
    const int num_disparities = 64; // 반드시 16의 배수여야 함
    const int block_size = 5;

public:
    explicit DepthEsmimator(
        const StereoCamera& stereo_camera,
        const int min_disparity = 0,
        const int num_disparities = 64,
        const int block_size = 5
    ):
        image_size{stereo_camera.get_frame_size()},
        map1x{stereo_camera.get_map1x()},
        map1y{stereo_camera.get_map1y()},
        map2x{stereo_camera.get_map2x()},
        map2y{stereo_camera.get_map2y()},
        Q{stereo_camera.get_Q()},
        min_disparity{min_disparity},
        num_disparities{num_disparities},
        block_size{block_size} {
    }

    void update(const cv::UMat& new_rectified_left, const cv::UMat& new_rectified_right) {
        assert((new_rectified_left.rows == image_size.height && new_rectified_left.cols == image_size.width &&
                new_rectified_right.rows == image_size.height && new_rectified_right.cols == image_size.width) &&
            "[DepthEstimator::update] 입력된 이미지 크기가 초기화된 이미지 크기와 일치하지 않습니다.");

        rectified_left = new_rectified_left;
        rectified_right = new_rectified_right;

        cv::cvtColor(rectified_left, gray_left, cv::COLOR_BGR2GRAY);
        cv::cvtColor(rectified_right, gray_right, cv::COLOR_BGR2GRAY);

        // Initialize StereoSGBM matcher if not already done
        if (!stereo_matcher) {
            stereo_matcher = cv::StereoSGBM::create(min_disparity, num_disparities, block_size);
        }

        // Scale disparity to float
        cv::UMat disparity_temp;
        stereo_matcher->compute(gray_left, gray_right, disparity_temp);
        disparity_temp.convertTo(disparity, CV_32F, 1.0 / 16.0);
    }

    [[nodiscard]] const cv::UMat& get_disparity() const {
        return disparity;
    }

    [[nodiscard]] std::optional<float> get_depth_at(const cv::Point2i& point) const {
        assert(!disparity.empty() && "[DepthEstimator::get_depth_at] 먼저 update()를 호출해야 합니다.");
        assert(
            (0 <= point.x && point.x < image_size.width && 0 <= point.y && point.y < image_size.height) &&
            "[DepthEstimator::get_depth_at] 입력된 좌표가 이미지 크기를 벗어났습니다.");

        const float disp = disparity.getMat(cv::ACCESS_READ).at<float>(point.y, point.x);
        if (disp <= 0.0f) return std::nullopt; // invalid disparity

        cv::Matx44f Q_mat = Q.getMat(cv::ACCESS_READ);
        const float Q_32 = Q_mat(3, 2);
        const float Q_33 = Q_mat(3, 3);

        return Q_32 / (disp + Q_33);
    }

    [[nodiscard]] std::optional<cv::Point3f> pos_2d_to_3d(const cv::Point2i& pt) const {
        assert(!disparity.empty() && "[DepthEstimator::pos_2d_to_3d] 먼저 update()를 호출해야 합니다.");

        const float disp = disparity.getMat(cv::ACCESS_READ).at<float>(pt.y, pt.x);
        if (disp <= 0.0f) return std::nullopt; // invalid

        const cv::Matx44f Q_mat = Q.getMat(cv::ACCESS_READ);
        const cv::Vec4f input(static_cast<float>(pt.x), static_cast<float>(pt.y), disp, 1.0f);
        cv::Vec4f result = Q_mat * input;

        const float w = result[3];
        return cv::Point3f(result[0] / w, result[1] / w, result[2] / w);
    }

    [[nodiscard]] const cv::UMat& get_rectified_left() const {
        return rectified_left;
    }

    [[nodiscard]] const cv::UMat& get_rectified_right() const {
        return rectified_right;
    }

    [[nodiscard]] const cv::UMat& get_gray_left() const {
        return gray_left;
    }

    [[nodiscard]] const cv::UMat& get_gray_right() const {
        return gray_right;
    }

    [[nodiscard]] const cv::UMat& get_map1x() const {
        return map1x;
    }

    [[nodiscard]] const cv::UMat& get_map1y() const {
        return map1y;
    }

    [[nodiscard]] const cv::UMat& get_map2x() const {
        return map2x;
    }

    [[nodiscard]] const cv::UMat& get_map2y() const {
        return map2y;
    }

    [[nodiscard]] const cv::UMat& get_Q() const {
        return Q;
    }
};

#endif //DEPTH_ESTIMATOR_HPP
