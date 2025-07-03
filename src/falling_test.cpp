#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include "constants.hpp"

class VideoCaptureThreaded {
public:
    VideoCaptureThreaded(int src = 0)
        : cap(src), stopped(false)
    {
        if (!cap.isOpened())
            throw std::runtime_error("Cannot open camera");
        // 첫 프레임 미리 읽어 두기
        cap >> frame;
    }

    void start() {
        worker = std::thread(&VideoCaptureThreaded::update, this);
        // 시작 직후에도 frame이 비어 있으면 잠깐 대기
        while (frame.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    cv::Mat read() {
        std::lock_guard<std::mutex> lock(mtx);
        return frame.clone();
    }

    void stop() {
        stopped = true;
        if (worker.joinable())
            worker.join();
    }

private:
    void update() {
        cv::Mat tmp;
        while (!stopped) {
            if (!cap.read(tmp) || tmp.empty()) {
                // 빈 프레임이면 그냥 다음 루프
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            std::lock_guard<std::mutex> lock(mtx);
            tmp.copyTo(frame);
        }
    }

    cv::VideoCapture cap;
    cv::Mat frame;
    std::thread worker;
    std::mutex mtx;
    std::atomic<bool> stopped;
};

int main() {
    try {
        // -- 기존 cap 대신 스레드 캡처 사용
        VideoCaptureThreaded cap(0);
        cap.start();

        auto last_time = std::chrono::high_resolution_clock::now();
        int frame_count = 0, fps = 0;
        cv::TickMeter tm;
        std::deque<cv::Point2f> trails;

        while (true) {
            // 백그라운드 스레드에서 프레임 읽기
            cv::Mat frame = cap.read();
            if (frame.empty()) {
                std::cerr << "프레임을 불러올 수 없습니다." << std::endl;
                break;
            }

            // FPS 계산
            tm.reset(); tm.start();
            frame_count++;
            auto current_time = std::chrono::high_resolution_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();
            if (elapsed_ms >= 1000) {
                fps = frame_count * 1000 / elapsed_ms;
                last_time = current_time;
                frame_count = 0;
            }

            // 이하 기존 색상 검출·컨투어·표시 로직 동일
            cv::Mat mask, hsv, display;
            cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
            cv::inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            frame.copyTo(display);

            bool is_touched = false, is_detected = false;
            for (const auto& contour : contours) {
                if (cv::contourArea(contour) < CONTOUR_AREA_MIN) continue;
                double area = cv::contourArea(contour);
                double perimeter = cv::arcLength(contour, true);
                double circularity = 4 * CV_PI * area / (perimeter * perimeter);
                if (circularity < CIRCULARITY_MIN) continue;

                cv::Point2f center; float radius;
                cv::minEnclosingCircle(contour, center, radius);
                if (radius > RADIUS_MIN) {
                    is_detected = true;
                    trails.push_back(center);
                    if (trails.size() > TRAILS_SIZE) trails.pop_front();

                    cv::circle(display, center, (int)radius, cv::Scalar(0,0,255), 2);
                    cv::circle(display, center, 2, cv::Scalar(255,0,0), -1);
                    for (int i = 0; i < (int)trails.size()-1; ++i) {
                        cv::line(display, trails[i], trails[i+1], cv::Scalar(255,0,0),
                                 1 + 4 * ((double)i / trails.size()));
                    }
#ifdef PRINT
                    std::cout << "Ball (r="<<radius<<", x="<<center.x<<", y="<<center.y<<")\n";
#endif
                    if (TOUCHED_MIN < center.y && center.y < TOUCHED_MAX) {
                        is_touched = true;
#ifdef PRINT
                        std::cout << "Ball touched at y="<<center.y<<"\n";
#endif
                        std::cout << "\a";
                    }
                    break;
                }
            }

            // 프레임 표시
            std::string fps_text = "FPS: " + std::to_string(fps) + "/" +
                                   std::to_string((int)cap.read().rows);
            cv::putText(display, fps_text, {10,50},
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, {0,0,255}, 2);
            tm.stop();

            cv::imshow("Low-Res Webcam with FPS", display);
            cv::imshow("Orange Detection", mask);
            if (cv::waitKey(1) == 'q') break;
        }

        cap.stop();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    cv::destroyAllWindows();
    return 0;
}