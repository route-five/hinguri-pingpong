//
// Created by Hyunseung Ryu on 2025. 7. 25..
//

#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <vector>
#include <matplot/matplot.h>

#include "../utils/constants.hpp"

namespace plt = matplot;

class Visualizer {
private:
    // clang-format off
    const std::vector<double> table_x = {0, 0,            TABLE_WIDTH,  TABLE_WIDTH,  0};
    const std::vector<double> table_y = {0, TABLE_HEIGHT, TABLE_HEIGHT, 0,            0};
    const std::vector<double> table_z = {0, 0,            0,            0,            0};
    const std::vector<double> net_x = {0,                 TABLE_WIDTH,      TABLE_WIDTH,      0,                0};
    const std::vector<double> net_y = {TABLE_HEIGHT / 2,  TABLE_HEIGHT / 2, TABLE_HEIGHT / 2, TABLE_HEIGHT / 2, TABLE_HEIGHT / 2};
    const std::vector<double> net_z = {0,                 0,                TABLE_NET_HEIGHT, TABLE_NET_HEIGHT, 0};
    const std::vector<double> center_x = {TABLE_WIDTH / 2, TABLE_WIDTH / 2};
    const std::vector<double> center_y = {0, TABLE_HEIGHT};
    const std::vector<double> center_z = {0, 0};
    // clang-format on

    std::vector<double> target_x, target_y, target_z;

public:
    Visualizer() {
        plt::hold(true);
    }

    ~Visualizer() = default;

    void add_point(cv::Point3f position) {
        const auto [x, y, z] = position;

        if (x < 0 || x > TABLE_WIDTH || y < 0 || y > TABLE_HEIGHT || z < 0) return;
        target_x.push_back(x);
        target_y.push_back(y);
        target_z.push_back(z);
    }

    void show() const {
        plt::plot3(table_x, table_y, table_z)->line_width(2).color("r");
        plt::plot3(net_x, net_y, net_z)->line_width(2).color("r");
        plt::plot3(center_x, center_y, center_z)->line_width(2).color("r");

        plt::scatter3(target_x, target_y, target_z)
            ->marker_face(true)
            .marker_size(2)
            .marker_face_color("b")
            .marker_color("b");

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
};

#endif //VISUALIZER_HPP
