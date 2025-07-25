#include "simulation/simulator_side.hpp"
#include "simulation/simulator_top.hpp"

int main() {
  Simulator::Top top("Top View");
  Simulator::Side side("Side View");

  std::cout << "Press 'r' to rerun or 'q' to exit." << std::endl;

  char key = 'r';
  while (true) {
    if (key == 'q') break;
    if (key == 'r') {
      top.initialize();
      top.render();
      side.initialize();
      side.render();
    }

    key = static_cast<char>(cv::waitKey(0));
  }

  return 0;
}
