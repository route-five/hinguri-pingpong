#include <iostream>
#include <opencv2/opencv.hpp>
#include "random/Random.hpp"

#define DEBUG

const float TABLE_WIDTH = 152.5;
const float TABLE_HEIGHT = 274;
const float scale = 3.0f;
const int border = 50;

cv::Point2i to_pixel(const cv::Vec2f &pt) {
    return {
        static_cast<int>(pt[0] * scale) + border,
        static_cast<int>((TABLE_HEIGHT - pt[1]) * scale) + border
    };
}

void render_pingpong_table(cv::Mat &img, const cv::Vec2f vertices[4]) {
    for (int i = 0; i < 4; ++i) {
        cv::line(img, to_pixel(vertices[i]), to_pixel(vertices[(i + 1) % 4]), cv::Scalar(0, 0, 0), 2);
    }

    // 좌표축 (수학적 좌표계 기준, y 증가 방향 위쪽)
    constexpr int arrow_size = 40;
    cv::arrowedLine(img, {border, img.rows - border}, {border + arrow_size, img.rows - border}, cv::Scalar(0, 0, 0), 2);
    cv::arrowedLine(img, {border, img.rows - border}, {border, img.rows - border - arrow_size}, cv::Scalar(0, 0, 0), 2);
    cv::putText(img, "x", {border + arrow_size + 5, img.rows - border - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 0, 0), 1);
    cv::putText(img, "y", {border - 15, img.rows - border - arrow_size - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 0, 0), 1);
}

std::pair<cv::Vec2f, float> render_initial_state(cv::Mat &img, const cv::Vec2f vertices[4]) {
    Random<float> random;

    // 시작점 위치 랜덤으로 정하기
    const cv::Vec2f start = random.generate() * (vertices[2] - vertices[1]) + vertices[1];

    // 각도 랜덤으로 정하기
    cv::Vec2f to_left = vertices[0] - start;
    cv::Vec2f to_right = vertices[3] - start;
    float angle_min = std::atan2(to_left[1], to_left[0]);
    float angle_max = std::atan2(to_right[1], to_right[0]);
    float angle = random.generate() * (angle_max - angle_min) + angle_min;

#ifdef DEBUG
    // 방향 범위 삼각형 그리기
    cv::Vec2f vec_left = start + cv::Vec2f{std::cos(angle_min) / std::sin(angle_min), 1} * TABLE_HEIGHT;
    cv::Vec2f vec_right = start + cv::Vec2f{std::cos(angle_max) / std::sin(angle_max), 1} * TABLE_HEIGHT;

    constexpr double alpha = 0.5;
    const std::vector triangle = {to_pixel(start), to_pixel(vec_left), to_pixel(vec_right)};

    cv::Mat overlay = img.clone();
    cv::fillConvexPoly(overlay, triangle, cv::Scalar(0, 255, 0), cv::LINE_AA);
    cv::addWeighted(overlay, alpha, img, 1.0 - alpha, 0, img);

    // 방향 범위 경계선 그리기
    cv::line(img, to_pixel(start), to_pixel(vec_left), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    cv::line(img, to_pixel(start), to_pixel(vec_right), cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
#endif

    // 시작점
    cv::circle(img, to_pixel(start), 6, cv::Scalar(0, 0, 255), -1);

    // 궤적
    const cv::Vec2f end = start + cv::Vec2f{std::cos(angle) / std::sin(angle), 1} * TABLE_HEIGHT;
    cv::arrowedLine(img, to_pixel(start), to_pixel(end), cv::Scalar(255, 0, 0), 2, cv::LINE_AA, 0, 0.02);

    return {start, angle};
}

void render(cv::Mat &img) {
    img.setTo(cv::Scalar(255, 255, 255));

    const cv::Vec2f vertices[4] = {{0, TABLE_HEIGHT}, {0, 0}, {TABLE_WIDTH, 0}, {TABLE_WIDTH, TABLE_HEIGHT}};

    render_pingpong_table(img, vertices);
    auto [start_pos, angle] = render_initial_state(img, vertices);
}

int main() {
    const int img_w = static_cast<int>(TABLE_WIDTH * scale) + 2 * border;
    const int img_h = static_cast<int>(TABLE_HEIGHT * scale) + 2 * border;
    cv::Mat img(img_h, img_w, CV_8UC3);

    std::cout << "Press 'r' to rerun or 'q' to exit.\n";

    char key = 0;
    while (key != 'q') {
        render(img);
        cv::imshow("Ping Pong Vector", img);

        key = static_cast<char>(cv::waitKey(0));
    }

    return 0;
}
