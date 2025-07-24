#include <matplot/matplot.h>

#include "../../src/utils/constants.hpp"

namespace plt = matplot;

int main() {
  using namespace std;

  // 단위 변환: cm → m
  double table_width = TABLE_WIDTH / 100.0;
  double table_height = TABLE_HEIGHT / 100.0;
  double net_height = TABLE_NET_HEIGHT / 100.0;

  // 테이블 4 모서리 좌표
  vector<double> x = {0, table_height, table_height, 0, 0};
  vector<double> y = {0, 0, table_width, table_width, 0};
  vector<double> z = {0, 0, 0, 0, 0};

  // 테이블 바닥 테두리
  plt::hold(true);  // 여러 plot3를 같은 figure에 유지
  plt::plot3(x, y, z)->line_width(2).color("r");

  // 네트 위치 (가운데에 세움)
  double net_x = table_height / 2.0;
  vector<double> net_y = {0, table_width};
  vector<double> net_x_vec(2, net_x);

  // 양 끝 기둥
  vector<double> net_z_base = {0, net_height};
  for (double y_val : net_y) {
    vector<double> y_vec(2, y_val);
    plt::plot3(net_x_vec, y_vec, net_z_base)->line_width(2).color("r");
  }

  // 바닥 연결선
  vector<double> net_z_bottom = {0, 0};
  plt::plot3(net_x_vec, net_y, net_z_bottom)->line_width(2).color("r");

  // 위쪽 연결선
  vector<double> net_z_top = {net_height, net_height};
  plt::plot3(net_x_vec, net_y, net_z_top)->line_width(2).color("r");

  // 탁구공 위치 시각화 (포물선 궤적)
  vector<double> ball_x, ball_y, ball_z;
  double y0 = 0.7;    // 초기 y 위치
  double z0 = 0.2;    // 초기 높이 (m)
  double vx = 3.0;    // x 속도 (m/s)
  double vz = 3.5;    // z 속도 (m/s)
  double g = 9.8;     // 중력 가속도 (m/s^2)
  int N = 30;         // 점 개수
  double dt = 0.025;  // 시간 간격

  for (int i = 0; i < N; ++i) {
    double t = i * dt;
    ball_x.push_back(t * vx);
    ball_y.push_back(y0);
    ball_z.push_back(z0 + vz * t - 0.5 * g * t * t);
  }

  plt::scatter3(ball_x, ball_y, ball_z)
      ->marker_face(true)
      .marker_face_color("b")
      .marker_color("b");

  // 전체 보기 설정
  plt::xlabel("X [m]");
  plt::ylabel("Y [m]");
  plt::zlabel("Z [m]");
  plt::title("3D Table Tennis Table");
  plt::grid(true);
  plt::xlim({-0.5, table_height + 0.5});
  plt::ylim({-0.5, table_width + 0.5});
  plt::zlim({-0.5, 1.5});

  plt::show();
}