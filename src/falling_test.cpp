#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

#include "constants.hpp"

#define PRINT

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera" << std::endl;
        return -1;
    }

    // cap prop
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 32);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 32);
    cap.set(cv::CAP_PROP_FPS, 120);

    cv::Mat frame, display;
    auto last_time = std::chrono::high_resolution_clock::now();
    int frame_count = 0;
    int fps = 0;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "프레임을 불러올 수 없습니다." << std::endl;
            break;
        }

        // calculate fps
        frame_count++;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto time_since_last_update = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();
        if (time_since_last_update >= 1000) {
            fps = frame_count * 1000 / time_since_last_update;
            last_time = current_time;
            frame_count = 0;
        }

        // color detection
        cv::Mat mask, hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);

        // contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        frame.copyTo(display);
        bool is_touched = false, is_detected = false;

        for (const auto& contour : contours) {
            if (cv::contourArea(contour) < 100) continue; // ignore small areas

            double area = cv::contourArea(contour);
            double perimeter = cv::arcLength(contour, true);
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity < 0.5) continue;

            cv::Point2f center;
            float radius;
            cv::minEnclosingCircle(contour, center, radius);

            if (radius > 10) {  // 네트에 닿아있을 때 radius = 10.6~7쯤 나옴
                is_detected = true;
                cv::circle(display, center, static_cast<int>(radius), cv::Scalar(0, 0, 255), 2);
                cv::circle(display, center, 2, cv::Scalar(255, 0, 0), -1);
#ifdef PRINT
                std::cout << "Ball (radius=" << radius << ", x=" << center.x << ", y=" << center.y << ")" << std::endl;
#endif

                if (210 < center.y && center.y < 230) {
                    is_touched = true;
#ifdef PRINT
                    std::cout << "Ball touched the ground at y=" << center.y << std::endl;
#endif
                    std::cout << "\a"; // Beep sound
                }

                break;
            }
        }

        // save frame image
#ifdef SAVE_IMAGE
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();
        long duration = value.count() % 1000;

        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time_t);

        char time_buffer[64];
        std::strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &now_tm);
        std::string filename = std::string(time_buffer) + "_" + std::to_string(duration) + (is_detected ? "_detected" : "") + (is_touched ? "_touch" : "") + ".png";

        cv::imwrite("img/test2/original/" + filename, frame);
        cv::imwrite("img/test2/detect/" + filename, display);
        // std::cout << filename << " saved!" << std::endl;
#endif

        // window
        std::string fpsText = "FPS: " + std::to_string(fps);
        cv::putText(display, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
                    1.0, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Low-Res Webcam with FPS", display);
        cv::imshow("Orange Detection", mask);

        if (cv::waitKey(1) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
