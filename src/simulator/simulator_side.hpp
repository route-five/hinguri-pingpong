//
// Created by Hyunseung Ryu on 2025. 7. 3..
//

#ifndef SIMULATOR_SIDE_HPP
#define SIMULATOR_SIDE_HPP

#include <csetjmp>
#include <numbers>
#include <opencv2/opencv.hpp>

#include "../constants.hpp"
#include "../random/Random.hpp"
#include "simulator_property.hpp"

// TODO: init_pos, init_vel, init_angle 없이 점 3개로 quadratic regression을
//  하는 것도 구현하기

// TODO: 튕겼을 때 pos, vel, angle 다시 계산해서 z(x)도 다시 바꿔야 함

// TODO: min_speed, max_speed 계산 다시 하기

class SimulatorSide {
 private:
  std::string window_name;
  const SimulatorProperty property;

  cv::Mat img;
  cv::Vec2f vertices[4] = {{0, TABLE_Z_SIZE},
                           {0, 0},
                           {TABLE_X_SIZE, 0},
                           {TABLE_X_SIZE, TABLE_Y_SIZE}};
  cv::Vec2f init_pos;
  cv::Vec2f init_vel;
  cv::Vec2f init_max_vel;
  cv::Vec2f init_min_vel;
  float init_angle;

  float z(const float x) {
    return init_pos[1] + x * std::tan(init_angle) -
           GRAVITY * x * x / (2 * cv::pow(init_vel[0], 2));
  }

  float z(const float x, const cv::Vec2f &vel) {
    return init_pos[1] + x * std::tan(init_angle) -
           GRAVITY * x * x / (2 * cv::pow(vel[0], 2));
  }

  [[nodiscard]] cv::Point2i to_pixel(const cv::Vec2f &pt) const {
    return property.to_pixel(pt, TABLE_Z_SIZE);
  }

  [[nodiscard]] static cv::Scalar_<int> color(const int R, const int G,
                                              const int B) {
    return cv::Scalar(B, G, R);
  }

  void render_background() { img.setTo(cv::Scalar(255, 255, 255)); }

  void render_pingpong_table() const {
    // 탁구대 그리기
    cv::line(img, to_pixel(vertices[1]), to_pixel(vertices[2]), color(0, 0, 0),
             2);

    // 중앙 네트 라인 그리기
    const cv::Vec2f mid_top = {TABLE_X_SIZE / 2, TABLE_NET_HEIGHT};
    const cv::Vec2f mid_down = {TABLE_X_SIZE / 2, 0};
    cv::line(img, to_pixel(mid_top), to_pixel(mid_down), color(80, 80, 80), 1,
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
        img, "z",
        {property.margin - 15, img.rows - property.margin - arrow_size - 5},
        cv::FONT_HERSHEY_SIMPLEX, 0.5, color(0, 0, 0), 1);
  }

  void render_trajectory() {
    // 속도 영역 그리기
    cv::Mat overlay = img.clone();
    constexpr double alpha = 0.1;
    std::vector<cv::Point> polygon_points;
    for (float x = 0; x <= TABLE_X_SIZE; ++x) {
      polygon_points.push_back(to_pixel({x, z(x, init_min_vel)}));
    }
    for (float x = TABLE_X_SIZE; x >= 0; x -= 1.0f) {
      polygon_points.push_back(to_pixel({x, z(x, init_max_vel)}));
    }
    cv::fillPoly(overlay, {polygon_points}, color(0, 0, 255));
    cv::addWeighted(overlay, alpha, img, 1.0 - alpha, 0, img);

    // 포물선 궤적
    for (float x = 0; x <= TABLE_X_SIZE; ++x) {
      cv::circle(img, to_pixel({x, z(x)}), 2, color(0, 0, 255), -1);
      cv::circle(img, to_pixel({x, z(x, init_min_vel)}), 1, color(0, 0, 255),
                 -1);
      cv::circle(img, to_pixel({x, z(x, init_max_vel)}), 1, color(0, 0, 255),
                 -1);
    }
    // 시작점
    cv::circle(img, to_pixel(init_pos), 6, color(255, 0, 0), -1);

    // 초기 각도 화살표 그리기
    constexpr float arrow_length = 20.0f;
    const cv::Vec2f arrow_dir = {std::cos(init_angle) * arrow_length,
                                 std::sin(init_angle) * arrow_length};
    const cv::Point2i arrow_start = to_pixel(init_pos);
    const cv::Point2i arrow_end = to_pixel(init_pos + arrow_dir);
    cv::arrowedLine(img, arrow_start, arrow_end, color(0, 200, 0), 2,
                    cv::LINE_AA);
  }

 public:
  void initialize() {
    Random<float> random;

    constexpr float angle_min = -std::numbers::pi_v<float> / 4;
    constexpr float angle_max = std::numbers::pi_v<float> / 4;
    constexpr float z_net = TABLE_NET_HEIGHT;
    constexpr float x_table = TABLE_X_SIZE;
    constexpr float z_table = TABLE_Z_SIZE;

    do {
      // 각도는 -45도 이상 ~ 45도 미만
      init_angle = random.generate() * (angle_max - angle_min) + angle_min;

      init_pos =
          random.generate() * (vertices[0] / 2 - vertices[1]) + vertices[1];

      // 최소 속력 = 네트를 넘는 데 필요한 속력
      const float tan_theta = std::tan(init_angle);
      const float cos_theta = std::cos(init_angle);

      const float dz_net = init_pos[1] - z_net;
      const float dz_table = init_pos[1] - z_table;

      const float min_speed_sq =
          GRAVITY * x_table * x_table /
          (2 * std::pow(cos_theta, 2) * (x_table * tan_theta + dz_net));

      // 최대 속력 = 탁구대에 아웃되기 직전의 속력
      const float max_speed_sq =
          GRAVITY * x_table * x_table /
          (2 * std::pow(cos_theta, 2) * (x_table * tan_theta + dz_table));

      if (min_speed_sq > 0 && max_speed_sq > min_speed_sq) {
        const float min_speed = std::sqrt(min_speed_sq);
        const float max_speed = std::sqrt(max_speed_sq);

        const float speed =
            random.generate() * (max_speed - min_speed) + min_speed;

        init_vel = {speed * std::cos(init_angle), speed * std::sin(init_angle)};
        init_max_vel = {max_speed * std::cos(init_angle),
                        max_speed * std::sin(init_angle)};
        init_min_vel = {min_speed * std::cos(init_angle),
                        min_speed * std::sin(init_angle)};
        break;
      }
    } while (true);
  }

  explicit SimulatorSide(
      const std::string &window_name, const cv::Vec2f &init_pos,
      const cv::Vec2f &init_vel, const float init_angle,
      const SimulatorProperty &property = SimulatorProperty())
      : window_name{window_name},
        property{property},
        init_pos{init_pos},
        init_vel{init_vel},
        init_angle{init_angle} {
    const int img_w =
        static_cast<int>(TABLE_X_SIZE * property.scale) + 2 * property.margin;
    const int img_h =
        static_cast<int>(TABLE_Z_SIZE * property.scale) + 2 * property.margin;
    img = cv::Mat(img_h, img_w, CV_8UC3);
  }

  explicit SimulatorSide(
      const std::string &window_name,
      const SimulatorProperty &property = SimulatorProperty())
      : window_name{window_name}, property{property} {
    const int img_w =
        static_cast<int>(TABLE_X_SIZE * property.scale) + 2 * property.margin;
    const int img_h =
        static_cast<int>(TABLE_Z_SIZE * property.scale) + 2 * property.margin;
    img = cv::Mat(img_h, img_w, CV_8UC3);

    initialize();
  }

  void render(const int arrow_size = 20) {
    render_background();
    render_pingpong_table();
    render_axis(arrow_size);
    render_trajectory();

    cv::imshow(window_name, img);
  }
};

#endif  // SIMULATOR_SIDE_HPP
