#include <matplot/matplot.h>

#include "../../src/utils/constants.hpp"

namespace plt = matplot;

int main() {
  using namespace std;

  // clang-format off
  vector<double> table_y_coords = {0, TABLE_HEIGHT, TABLE_HEIGHT, 0,            0};
  vector<double> table_x_coords = {0, 0,            TABLE_WIDTH,  TABLE_WIDTH,  0};
  vector<double> table_z_coords = {0, 0,            0,            0,            0};
  // clang-format on

  // clang-format off
  vector<double> net_x_coords = {0,                 TABLE_WIDTH,      TABLE_WIDTH,      0,                0};
  vector<double> net_y_coords = {TABLE_HEIGHT / 2,  TABLE_HEIGHT / 2, TABLE_HEIGHT / 2, TABLE_HEIGHT / 2, TABLE_HEIGHT / 2};
  vector<double> net_z_coords = {0,                 0,                TABLE_NET_HEIGHT, TABLE_NET_HEIGHT, 0};
  // clang-format on

  plt::hold(true);
  plt::plot3(table_x_coords, table_y_coords, table_z_coords)->line_width(2).color("r");
  plt::plot3(net_x_coords, net_y_coords, net_z_coords)->line_width(2).color("r");

  // 탁구공 위치 시각화 (포물선 궤적)
  vector<double> ball_x_traj, ball_y_traj, ball_z_traj;
  double y0 = 70; // 초기 y 위치 (cm)
  double z0 = 20; // 초기 높이 (cm)
  double vx = 300; // x 속도 (cm/s)
  double vz = 350; // z 속도 (cm/s)
  double g = 980.0; // 중력 가속도 (cm/s^2)
  int N = 30; // 점 개수
  double dt = 0.025; // 시간 간격

  for (int i = 0; i < N; ++i) {
    double t = i * dt;
    ball_y_traj.push_back(t * vx);
    ball_x_traj.push_back(y0);
    ball_z_traj.push_back(z0 + vz * t - 0.5 * g * t * t);
  }

  plt::scatter3(ball_x_traj, ball_y_traj, ball_z_traj)
    ->marker_face(true)
    .marker_face_color("b")
    .marker_color("b");

  // 전체 보기 설정
  plt::xlabel("X [cm]");
  plt::ylabel("Y [cm]");
  plt::zlabel("Z [cm]");
  plt::title("3D Table Tennis Table");
  plt::grid(true);
  plt::ylim({-50, TABLE_HEIGHT + 50});
  plt::xlim({-50, TABLE_WIDTH + 50});
  plt::zlim({0, 150});

  plt::show();
}
