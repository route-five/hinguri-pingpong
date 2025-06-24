#include <iostream>
#include <numbers>
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

void render_background(cv::Mat &img) {
    img.setTo(cv::Scalar(255, 255, 255));
}

void render_pingpong_table(cv::Mat &img, const cv::Vec2f vertices[4]) {
    for (int i = 0; i < 4; ++i) {
        cv::line(img, to_pixel(vertices[i]), to_pixel(vertices[(i + 1) % 4]), cv::Scalar(0, 0, 0), 2);
    }
}

void render_axis(cv::Mat &img) {
    constexpr int arrow_size = 40;
    cv::arrowedLine(img, {border, img.rows - border}, {border + arrow_size, img.rows - border}, cv::Scalar(0, 0, 0), 2);
    cv::arrowedLine(img, {border, img.rows - border}, {border, img.rows - border - arrow_size}, cv::Scalar(0, 0, 0), 2);
    cv::putText(img, "x", {border + arrow_size + 5, img.rows - border - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 0, 0), 1);
    cv::putText(img, "y", {border - 15, img.rows - border - arrow_size - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 0, 0), 1);
}

std::tuple<cv::Vec2f, cv::Vec2f, float> render_initial_state(cv::Mat &img, const cv::Vec2f vertices[4]) {
    Random<float> random;

    // 시작점 위치 랜덤으로 정하기
    const cv::Vec2f init_pos = random.generate() * (vertices[2] - vertices[1]) + vertices[1];

    // 각도 랜덤으로 정하기
    cv::Vec2f to_left = vertices[0] - init_pos;
    cv::Vec2f to_right = vertices[3] - init_pos;
    float angle_min = std::atan2(to_left[1], to_left[0]);
    float angle_max = std::atan2(to_right[1], to_right[0]);
    float angle = random.generate() * (angle_max - angle_min) + angle_min;

#ifdef DEBUG
    // 방향 범위 삼각형 그리기
    cv::Vec2f vec_left = init_pos + cv::Vec2f{std::cos(angle_min) / std::sin(angle_min), 1} * TABLE_HEIGHT;
    cv::Vec2f vec_right = init_pos + cv::Vec2f{std::cos(angle_max) / std::sin(angle_max), 1} * TABLE_HEIGHT;

    constexpr double alpha = 0.1;
    const std::vector triangle = {to_pixel(init_pos), to_pixel(vec_left), to_pixel(vec_right)};

    cv::Mat overlay = img.clone();
    cv::fillConvexPoly(overlay, triangle, cv::Scalar(255, 0, 0), cv::LINE_AA);
    cv::addWeighted(overlay, alpha, img, 1.0 - alpha, 0, img);

    // 방향 범위 경계선 그리기
    cv::line(img, to_pixel(init_pos), to_pixel(vec_left), cv::Scalar(255, 0, 0), 1, cv::LINE_AA);
    cv::line(img, to_pixel(init_pos), to_pixel(vec_right), cv::Scalar(255, 0, 0), 1, cv::LINE_AA);
#endif

    // 시작점
    cv::circle(img, to_pixel(init_pos), 6, cv::Scalar(0, 0, 255), -1);

    // 궤적
    const cv::Vec2f end_pos = init_pos + cv::Vec2f{std::cos(angle) / std::sin(angle), 1} * TABLE_HEIGHT;
    cv::arrowedLine(img, to_pixel(init_pos), to_pixel(end_pos), cv::Scalar(255, 0, 0), 2, cv::LINE_AA, 0, 0.02);

    return {init_pos, end_pos, angle};
}

void render_projected_trajectory(cv::Mat &img, const cv::Vec2f vertices[4], cv::Vec2f init_pos, cv::Vec2f end_pos,
                                 float angle) {
    // 대각선 그리기
    cv::line(img, to_pixel(vertices[0]), to_pixel(vertices[2]), cv::Scalar(0, 0, 0), 1, cv::LINE_AA);

    // 사영 벡터
    const cv::Vec2f trajectory = end_pos - init_pos;
    const cv::Vec2f diagonal = vertices[0] - vertices[2];
    const cv::Vec2f projected = trajectory.dot(diagonal) / (cv::norm(diagonal) * cv::norm(diagonal)) * diagonal;

    // offset
    const float camera_angle = std::atan2(TABLE_HEIGHT, TABLE_WIDTH);
    const float offset_angle = std::numbers::pi_v<float> / 2 - camera_angle;

    const float offset_x = (vertices[2] - init_pos)[0] * cos(offset_angle) * cos(offset_angle);
    const float offset_y = (vertices[2] - init_pos)[0] * cos(offset_angle) * sin(offset_angle);
    const cv::Vec2f offset = {offset_x, offset_y};

    const cv::Vec2f projected_trajectory[2] = {init_pos + offset, init_pos + offset + projected};

    cv::arrowedLine(img, to_pixel(projected_trajectory[0]), to_pixel(projected_trajectory[1]), cv::Scalar(0, 0, 255), 2,
                    cv::LINE_AA, 0, 0.02);
    cv::line(img, to_pixel(projected_trajectory[0]), to_pixel(init_pos), cv::Scalar(0, 128, 255), 1, cv::LINE_AA);
    cv::line(img, to_pixel(projected_trajectory[1]), to_pixel(end_pos), cv::Scalar(0, 128, 255), 1, cv::LINE_AA);
}

void render(cv::Mat &img) {
    const cv::Vec2f vertices[4] = {{0, TABLE_HEIGHT}, {0, 0}, {TABLE_WIDTH, 0}, {TABLE_WIDTH, TABLE_HEIGHT}};

    render_background(img);
    render_pingpong_table(img, vertices);
    render_axis(img);
    auto [init_pos, end_pos, angle] = render_initial_state(img, vertices);
    render_projected_trajectory(img, vertices, init_pos, end_pos, angle);
}

int main() {
    const int img_w = static_cast<int>(TABLE_WIDTH * scale) + 2 * border;
    const int img_h = static_cast<int>(TABLE_HEIGHT * scale) + 2 * border;
    cv::Mat img(img_h, img_w, CV_8UC3);

    std::cout << "Press 'r' to rerun or 'q' to exit.\n";

    char key = 'r';
    while (key != 'q') {
        if (key == 'r') {
            render(img);
        }

        cv::imshow("Ping Pong Simulator", img);

        key = static_cast<char>(cv::waitKey(0));
    }

    return 0;
}
