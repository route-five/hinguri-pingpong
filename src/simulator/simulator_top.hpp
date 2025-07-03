//
// Created by Hyunseung Ryu on 25. 6. 25.
//

#ifndef INC_SIMULATOR_2D_HPP
#define INC_SIMULATOR_2D_HPP

#include <numbers>
#include <opencv2/opencv.hpp>

#include "../Constants.hpp"
#include "../random/Random.hpp"
#include "simulator_property.hpp"

class SimulatorTop {
 private:
  std::string window_name;
  const SimulatorProperty property;

  cv::Mat img;
  cv::Vec2f vertices[4] = {{0, TABLE_Y_SIZE},
                           {0, 0},
                           {TABLE_X_SIZE, 0},
                           {TABLE_X_SIZE, TABLE_Y_SIZE}};
  cv::Vec2f init_pos;
  float init_angle;

  [[nodiscard]] cv::Point2i to_pixel(const cv::Vec2f &pt) const {
    return property.to_pixel(pt, TABLE_Y_SIZE);
  }

  [[nodiscard]] static cv::Scalar_<int> color(const int R, const int G,
                                              const int B) {
    return cv::Scalar(B, G, R);
  }

  void render_background() { img.setTo(cv::Scalar(255, 255, 255)); }

  void render_pingpong_table() const {
    // 탁구대 그리기
    for (int i = 0; i < 4; ++i) {
      cv::line(img, to_pixel(vertices[i]), to_pixel(vertices[(i + 1) % 4]),
               color(0, 0, 0), 2);
    }

    // 중앙 네트 라인 그리기
    const cv::Vec2f mid_left = {0, TABLE_Y_SIZE / 2};
    const cv::Vec2f mid_right = {TABLE_X_SIZE, TABLE_Y_SIZE / 2};
    cv::line(img, to_pixel(mid_left), to_pixel(mid_right), color(80, 80, 80), 1,
             cv::LINE_AA);
  }

  void render_axis(const int arrow_size) {
    cv::arrowedLine(img, {property.margin, img.rows - property.margin},
                    {property.margin + arrow_size, img.rows - property.margin},
                    color(0, 0, 0), 2);
    cv::arrowedLine(img, {property.margin, img.rows - property.margin},
                    {property.margin, img.rows - property.margin - arrow_size},
                    color(0, 0, 0), 2);
    cv::putText(
        img, "x",
        {property.margin + arrow_size + 5, img.rows - property.margin - 5},
        cv::FONT_HERSHEY_SIMPLEX, 0.5, color(0, 0, 0), 1);
    cv::putText(
        img, "y",
        {property.margin - 15, img.rows - property.margin - arrow_size - 5},
        cv::FONT_HERSHEY_SIMPLEX, 0.5, color(0, 0, 0), 1);
  }

  void render_trajectory() {
    cv::Vec2f to_left = vertices[0] - init_pos;
    cv::Vec2f to_right = vertices[3] - init_pos;
    const float angle_min = std::atan2(to_left[1], to_left[0]);
    const float angle_max = std::atan2(to_right[1], to_right[0]);

    // 방향 범위 삼각형 그리기
    const cv::Vec2f vec_left =
        init_pos +
        cv::Vec2f{std::cos(angle_min) / std::sin(angle_min), 1} * TABLE_Y_SIZE;
    const cv::Vec2f vec_right =
        init_pos +
        cv::Vec2f{std::cos(angle_max) / std::sin(angle_max), 1} * TABLE_Y_SIZE;

    constexpr double alpha = 0.1;
    const std::vector triangle = {to_pixel(init_pos), to_pixel(vec_left),
                                  to_pixel(vec_right)};

    cv::Mat overlay = img.clone();
    cv::fillConvexPoly(overlay, triangle, color(0, 0, 255), cv::LINE_AA);
    cv::addWeighted(overlay, alpha, img, 1.0 - alpha, 0, img);

    // 방향 범위 경계선 그리기
    cv::line(img, to_pixel(init_pos), to_pixel(vec_left), color(0, 0, 255), 1,
             cv::LINE_AA);
    cv::line(img, to_pixel(init_pos), to_pixel(vec_right), color(0, 0, 255), 1,
             cv::LINE_AA);

    // 시작점
    cv::circle(img, to_pixel(init_pos), 6, color(255, 0, 0), -1);

    // 궤적
    const cv::Vec2f end_pos =
        init_pos + cv::Vec2f{std::cos(init_angle) / std::sin(init_angle), 1} *
                       TABLE_Y_SIZE;
    cv::arrowedLine(img, to_pixel(init_pos), to_pixel(end_pos),
                    color(0, 0, 255), 2, cv::LINE_AA, 0, 0.02);
  }

  void render_projected_trajectory() {
    const cv::Vec2f end_pos =
        init_pos + cv::Vec2f{std::cos(init_angle) / std::sin(init_angle), 1} *
                       TABLE_Y_SIZE;

    // 대각선 그리기
    cv::line(img, to_pixel(vertices[0]), to_pixel(vertices[2]), color(0, 0, 0),
             1, cv::LINE_AA);

    // 사영 벡터
    const cv::Vec2f trajectory = end_pos - init_pos;
    const cv::Vec2f diagonal = vertices[0] - vertices[2];
    const cv::Vec2f projected = trajectory.dot(diagonal) /
                                (cv::norm(diagonal) * cv::norm(diagonal)) *
                                diagonal;

    // offset
    const float camera_angle = std::atan2(TABLE_Y_SIZE, TABLE_X_SIZE);
    const float offset_angle = std::numbers::pi_v<float> / 2 - camera_angle;
    const cv::Vec2f offset =
        cv::Vec2f{std::cos(offset_angle), std::sin(offset_angle)} *
        (TABLE_X_SIZE - init_pos[0]) * std::cos(offset_angle);

    const cv::Vec2f projected_trajectory[2] = {init_pos + offset,
                                               init_pos + offset + projected};
    cv::arrowedLine(img, to_pixel(projected_trajectory[0]),
                    to_pixel(projected_trajectory[1]), color(255, 0, 0), 2,
                    cv::LINE_AA, 0, 0.02);
    cv::line(img, to_pixel(projected_trajectory[0]), to_pixel(init_pos),
             color(255, 128, 0), 1, cv::LINE_AA);
    cv::line(img, to_pixel(projected_trajectory[1]), to_pixel(end_pos),
             color(255, 128, 0), 1, cv::LINE_AA);
  }

 public:
  void initialize() {
    Random<float> random;

    // 시작점 위치 랜덤으로 정하기
    init_pos = random.generate() * (vertices[2] - vertices[1]) + vertices[1];

    // 각도 랜덤으로 정하기
    cv::Vec2f to_left = vertices[0] - init_pos;
    cv::Vec2f to_right = vertices[3] - init_pos;
    const float angle_min = std::atan2(to_left[1], to_left[0]);
    const float angle_max = std::atan2(to_right[1], to_right[0]);
    init_angle = random.generate() * (angle_max - angle_min) + angle_min;
  }

  explicit SimulatorTop(const std::string &window_name,
                        const cv::Vec2f &init_pos, const float init_angle,
                        const SimulatorProperty &property = SimulatorProperty())
      : window_name{window_name},
        property{property},
        init_pos{init_pos},
        init_angle{init_angle} {
    const int img_w =
        static_cast<int>(TABLE_X_SIZE * property.scale) + 2 * property.margin;
    const int img_h =
        static_cast<int>(TABLE_Y_SIZE * property.scale) + 2 * property.margin;
    img = cv::Mat(img_h, img_w, CV_8UC3);
  }

  explicit SimulatorTop(const std::string &window_name,
                        const SimulatorProperty &property = SimulatorProperty())
      : window_name{window_name}, property{property} {
    const int img_w =
        static_cast<int>(TABLE_X_SIZE * property.scale) + 2 * property.margin;
    const int img_h =
        static_cast<int>(TABLE_Y_SIZE * property.scale) + 2 * property.margin;
    img = cv::Mat(img_h, img_w, CV_8UC3);

    initialize();
  }

  void render(const int arrow_size = 40) {
    render_background();
    render_pingpong_table();
    render_axis(arrow_size);
    render_trajectory();
    render_projected_trajectory();

    cv::imshow(window_name, img);
  }
};

#endif  // INC_SIMULATOR_2D_HPP
