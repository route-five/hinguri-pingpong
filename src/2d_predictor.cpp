#include "simulator/simulator_side.hpp"
#include "simulator/simulator_top.hpp"

int main() {
  SimulatorTop top("Top View");
  SimulatorSide side("Side View");

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
