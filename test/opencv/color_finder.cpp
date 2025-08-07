//
// Created by Hyunseung Ryu on 2025. 7. 27..
//

#include "../../src/vision/camera.hpp"
#include "../../src/vision/tracker.hpp"

int main() {
    Camera cam(CameraType::LEFT, {0});
    if (!cam.is_opened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    int h_min = 9, s_min = 70, v_min = 220;
    int h_max = 23, s_max = 255, v_max = 255;

    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Mask", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("H Min", "Mask", &h_min, 179);
    cv::createTrackbar("S Min", "Mask", &s_min, 255);
    cv::createTrackbar("V Min", "Mask", &v_min, 255);
    cv::createTrackbar("H Max", "Mask", &h_max, 179);
    cv::createTrackbar("S Max", "Mask", &s_max, 255);
    cv::createTrackbar("V Max", "Mask", &v_max, 255);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

    cam.start();

    while (true) {
        h_min = cv::getTrackbarPos("H Min", "Mask");
        s_min = cv::getTrackbarPos("S Min", "Mask");
        v_min = cv::getTrackbarPos("V Min", "Mask");
        h_max = cv::getTrackbarPos("H Max", "Mask");
        s_max = cv::getTrackbarPos("S Max", "Mask");
        v_max = cv::getTrackbarPos("V Max", "Mask");

        cv::Mat frame;
        cam >> frame;

        if (frame.empty()) continue;

        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        cv::Scalar lower_bound(h_min, s_min, v_min);
        cv::Scalar upper_bound(h_max, s_max, v_max);

        cv::Mat mask;
        cv::inRange(hsv, lower_bound, upper_bound, mask);

        // TODO: 노이즈 제거 필요한지 성능 및 효과 테스트 필요
        cv::erode(mask, mask, kernel);
        cv::dilate(mask, mask, kernel);

        // std::vector<std::vector<cv::Point>> contours;
        // cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        //
        // // min enclosing circle
        // for (const auto& contour : contours) {
        //     cv::Point2f center;
        //     float radius;
        //     cv::minEnclosingCircle(contour, center, radius);
        //     if (radius > RADIUS_MIN) {
        //         // 최소 반지름 조건
        //         cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 255, 0), 1);
        //         cv::circle(frame, center, 2, cv::Scalar(0, 0, 255), -1); // 중심점 표시
        //     }
        // }

        cv::imshow("Camera", frame);

        // HSV 그라디언트 시각화 바 생성
        cv::Mat hsv_bar(50, 256, CV_8UC3); // 높이 50, 너비 256
        for (int i = 0; i < 256; ++i) {
            int h = h_min + (h_max - h_min) * i / 255;
            int s = s_min + (s_max - s_min) * i / 255;
            int v = v_min + (v_max - v_min) * i / 255;

            cv::Mat hsv_pixel(1, 1, CV_8UC3, cv::Scalar(h, s, v));
            cv::Mat bgr_pixel;
            cv::cvtColor(hsv_pixel, bgr_pixel, cv::COLOR_HSV2BGR);
            cv::Vec3b bgr_color = bgr_pixel.at<cv::Vec3b>(0, 0);
            hsv_bar.col(i).setTo(bgr_color);
        }

        // Ensure hsv_bar has the same number of columns as mask before concatenation
        if (hsv_bar.cols != mask.cols) {
            cv::resize(hsv_bar, hsv_bar, cv::Size(mask.cols, hsv_bar.rows));
        }
        cv::Mat color_mask;
        color_mask.setTo(cv::Scalar(0, 0, 0)); // prevent drawing from frame leaking into color_mask
        cv::cvtColor(mask, color_mask, cv::COLOR_GRAY2BGR);
        cv::Mat combined;
        cv::vconcat(hsv_bar, color_mask, combined);
        cv::imshow("Mask", combined);

        // (위로 이동, see above)

        if (cv::waitKey(1) == 'q') break;
    }

    cam.stop();

    std::cout << "Optimal HSV Range:" << std::endl;
    std::cout << "Lower: (" << h_min << ", " << s_min << ", " << v_min << ")" << std::endl;
    std::cout << "Upper: (" << h_max << ", " << s_max << ", " << v_max << ")" << std::endl;

    cv::destroyAllWindows();

    return 0;
}
