//
// Created by Hyunseung Ryu on 2025. 7. 3..
//

#ifndef SIMULATOR_SIDE_HPP
#define SIMULATOR_SIDE_HPP

#include <numbers>
#include <opencv2/opencv.hpp>

#include "../constants.hpp"
#include "../random/random.hpp"
#include "simulator_property.hpp"

// TODO: init_pos, init_vel, init_angle 없이 점 3개로 quadratic regression을
//  하는 것도 구현하기

// TODO: 대각 포물선 그리기

namespace Simulator {
class Side {
 private:
  std::string window_name;
  const Property property;

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

  static float z(const float x, const cv::Vec2f &start_pos,
                 const cv::Vec2f &vel, const float angle) {
    return start_pos[1] + (x - start_pos[0]) * std::tan(angle) -
           GRAVITY * (x - start_pos[0]) * (x - start_pos[0]) /
               (2 * cv::pow(vel[0], 2));
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
             1, cv::LINE_AA);

    // 중앙 네트 라인 그리기
    const cv::Vec2f mid_top = {TABLE_X_SIZE / 2, TABLE_NET_HEIGHT};
    const cv::Vec2f mid_down = {TABLE_X_SIZE / 2, 0};
    cv::line(img, to_pixel(mid_top), to_pixel(mid_down), color(80, 80, 80), 1,
             cv::LINE_AA);
  }

  void render_axis(const int arrow_size) {
    cv::arrowedLine(img, {property.margin, img.rows - property.margin},
                    {property.margin + arrow_size, img.rows - property.margin},
                    color(0, 0, 0), 1, cv::LINE_AA);
    cv::arrowedLine(img, {property.margin, img.rows - property.margin},
                    {property.margin, img.rows - property.margin - arrow_size},
                    color(0, 0, 0), 1, cv::LINE_AA);
    cv::putText(
        img, "x",
        {property.margin + arrow_size + 5, img.rows - property.margin - 5},
        cv::FONT_HERSHEY_SIMPLEX, 0.5, color(0, 0, 0), 1, cv::LINE_AA);
    cv::putText(
        img, "z",
        {property.margin - 15, img.rows - property.margin - arrow_size - 5},
        cv::FONT_HERSHEY_SIMPLEX, 0.5, color(0, 0, 0), 1, cv::LINE_AA);
  }

  void render_trajectory() {
    // cv::Vec2f pos = init_pos;
    cv::Vec2f start_pos = init_pos;
    cv::Vec2f vel = init_vel;
    float angle = init_angle;

    constexpr float dx = 0.1f;
    for (float x = 0; x <= TABLE_X_SIZE; x += dx) {
      const float z = Side::z(x, start_pos, vel, angle);
      const float next_z = Side::z(x + dx, start_pos, vel, angle);

      // 바닥에 닿을 때
      if (z > 0 && next_z < 0) {
        const float bounced_pos = x + z / (z - next_z);

        vel = {vel[0], std::sqrt(2 * GRAVITY * start_pos[1] + vel[1] * vel[1]) *
                           TABLE_BOUNCE_COEFFICIENT};
        start_pos = {bounced_pos, 0};
        angle = std::atan2(vel[1], vel[0]);
      }

      cv::circle(img, to_pixel({x, z}), 1, color(0, 0, 255), 1, cv::LINE_AA);
    }

    // 속도 영역 그리기
    // constexpr double alpha = 0.1;
    // cv::Mat overlay = img.clone();
    // std::vector<cv::Point> polygon_points;
    //
    // for (float x = 0; x <= TABLE_X_SIZE; x += 1.0f) {
    //   polygon_points.push_back(
    //       to_pixel({x, std::max(0.0f, z(x, init_min_vel))}));
    // }
    // for (float x = TABLE_X_SIZE; x >= 0; x -= 1.0f) {
    //   polygon_points.push_back(
    //       to_pixel({x, std::max(0.0f, z(x, init_max_vel))}));
    // }
    //
    // cv::fillPoly(overlay, {polygon_points}, color(0, 0, 255));
    // cv::addWeighted(overlay, alpha, img, 1.0 - alpha, 0, img);

    // 포물선 궤적
    // for (float x = 0; x <= TABLE_X_SIZE; x += 1.0f) {
    //   cv::circle(img, to_pixel({x, std::max(0.0f, z(x))}), 2, color(0, 0,
    //   255),
    //              -1);
    //   cv::circle(img, to_pixel({x, std::max(0.0f, z(x, init_min_vel))}), 1,
    //              color(0, 0, 255), -1);
    //   cv::circle(img, to_pixel({x, std::max(0.0f, z(x, init_max_vel))}), 1,
    //              color(0, 0, 255), -1);
    // }

    // 시작점
    cv::circle(img, to_pixel(init_pos), 6, color(255, 0, 0), 1, cv::LINE_AA);

    // 초기 각도 화살표 그리기
    constexpr float arrow_length = 20.0f;
    const cv::Vec2f arrow_dir = {std::cos(init_angle) * arrow_length,
                                 std::sin(init_angle) * arrow_length};
    const cv::Point2i arrow_start = to_pixel(init_pos);
    const cv::Point2i arrow_end = to_pixel(init_pos + arrow_dir);
    cv::arrowedLine(img, arrow_start, arrow_end, color(0, 200, 0), 1,
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
      init_angle = random.generate(angle_min, angle_max);
      init_pos = {0, random.generate(0, TABLE_Z_SIZE)};

      const float H = init_pos[1];
      const float h = TABLE_NET_HEIGHT;
      const float L = TABLE_X_SIZE;
      const float g = GRAVITY;
      const float theta = init_angle;
      const float cos_theta = std::cos(theta);
      const float tan_theta = std::tan(theta);

      const float common = g * cv::pow(L / cos_theta, 2);

      const float net_speed_sq = common / (2 * (H + L * tan_theta));
      const float end_speed_sq = common / (4 * L * tan_theta + 8 * (H - h));

      const float min_speed_sq = std::min(net_speed_sq, end_speed_sq);
      const float max_speed_sq = std::max(net_speed_sq, end_speed_sq);

      if (min_speed_sq >= 0 && max_speed_sq >= 0) {
        const float min_speed = std::sqrt(min_speed_sq);
        const float max_speed = std::sqrt(max_speed_sq);

        const float init_speed = random.generate(min_speed, max_speed);
        const cv::Vec2f dir = {std::cos(init_angle), std::sin(init_angle)};

        init_vel = dir * init_speed;
        init_min_vel = dir * min_speed;
        init_max_vel = dir * max_speed;
        break;
      }
    } while (true);
  }

  explicit Side(const std::string &window_name, const cv::Vec2f &init_pos,
                const cv::Vec2f &init_vel, const float init_angle,
                const Property &property = Property())
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

  explicit Side(const std::string &window_name,
                const Property &property = Property())
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
}  // namespace Simulator

#endif  // SIMULATOR_SIDE_HPP
