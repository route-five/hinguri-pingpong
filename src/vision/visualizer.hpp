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
    // clang-format on

    std::vector<double> target_x, target_y, target_z;

public:
    Visualizer() = default;

    ~Visualizer() = default;

    void add_point(cv::Point3f position) {
        const auto [x, y, z] = position;

        target_x.push_back(x);
        target_y.push_back(y);
        target_z.push_back(z);
    }

    static bool is_point_in(const double x, const double y, const double z) {
        return 0 <= x && x <= TABLE_WIDTH &&
            0 <= y && y <= TABLE_HEIGHT &&
            0 <= z;
    }

    void show() const {
        auto f = plt::figure(false);
        f->backend()->run_command("unset warnings");
        plt::hold(true);
        plt::plot3(table_x, table_y, table_z)->line_width(2).color("r");
        plt::plot3(net_x, net_y, net_z)->line_width(2).color("r");

        std::vector<double> x_in, y_in, z_in;
        std::vector<double> x_out, y_out, z_out;

        for (size_t i = 0; i < target_x.size(); ++i) {
            if (is_point_in(target_x[i], target_y[i], target_z[i])) {
                x_in.push_back(target_x[i]);
                y_in.push_back(target_y[i]);
                z_in.push_back(target_z[i]);
            }
            else {
                x_out.push_back(target_x[i]);
                y_out.push_back(target_y[i]);
                z_out.push_back(target_z[i]);
            }
        }

        if (!x_in.empty()) {
            plt::scatter3(x_in, y_in, z_in)
                ->marker_face(true)
                .marker_face_color("b")
                .marker_color("b");
        }

        if (!x_out.empty()) {
            plt::scatter3(x_out, y_out, z_out)
                ->marker_face(true)
                .marker_face_color("g")
                .marker_color("g");
        }

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
