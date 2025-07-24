#include "simulation/simulator_2d.hpp"

int main() {
    Simulator2D sim("Ping Pong Simulator");

    std::cout << "Press 'r' to rerun or 'q' to exit." << std::endl;

    char key = 'r';
    while (true) {
        if (key == 'q')
            break;
        if (key == 'r')
            sim.render();

        key = static_cast<char>(cv::waitKey(0));
    }

    return 0;
}
