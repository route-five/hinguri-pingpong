#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <opencv2/opencv.hpp>
using namespace std;

// 2차 다항 회귀 함수
vector<double> quadraticFit(const vector<double>& times, const vector<double>& coords) {
    int n = times.size();
    double sumT = 0, sumT2 = 0, sumT3 = 0, sumT4 = 0;
    double sumY = 0, sumTY = 0, sumT2Y = 0;

    for (int i = 0; i < n; i++) {
        double t = times[i];
        double y = coords[i];
        sumT += t;
        sumT2 += t * t;
        sumT3 += t * t * t;
        sumT4 += t * t * t * t;

        sumY += y;
        sumTY += t * y;
        sumT2Y += t * t * y;
    }

    double A[3][3] = {
        {double(n), sumT, sumT2},
        {sumT, sumT2, sumT3},
        {sumT2, sumT3, sumT4}
    };

    double B[3] = {sumY, sumTY, sumT2Y};

    // 가우스 소거법
    for (int i = 0; i < 3; i++) {
        if (A[i][i] == 0) {
            for (int j = i + 1; j < 3; j++) {
                if (A[j][i] != 0) {
                    swap(A[i], A[j]);
                    swap(B[i], B[j]);
                    break;
                }
            }
        }
        double pivot = A[i][i];
        for (int j = 0; j < 3; j++) A[i][j] /= pivot;
        B[i] /= pivot;

        for (int j = 0; j < 3; j++) {
            if (i != j) {
                double factor = A[j][i];
                for (int k = 0; k < 3; k++) A[j][k] -= factor * A[i][k];
                B[j] -= factor * B[i];
            }
        }
    }

    return {B[2], B[1], B[0]};  // [a, b, c] 형태로 반환
}

double evaluateQuadratic(const vector<double>& coeffs, double t) {
    return coeffs[0]*t*t + coeffs[1]*t + coeffs[2];
}

// z(t)=0 되는 시간 추정
double estimateLandingTime(const vector<double>& z_coeff, const vector<double>& time_points) {
    double a = z_coeff[0];
    double b = z_coeff[1];
    double c = z_coeff[2];

    double discriminant = b*b - 4*a*c;
    double result_time = -1;

    if (discriminant >= 0) {
        double t1 = (-b + sqrt(discriminant)) / (2*a);
        double t2 = (-b - sqrt(discriminant)) / (2*a);
        double t_start = time_points.front();

        if (t1 > t_start && t2 > t_start)
            result_time = min(t1, t2);
        else if (t1 > t_start)
            result_time = t1;
        else if (t2 > t_start)
            result_time = t2;
    }

    if (result_time < 0) {
        double t_min = -b / (2*a);
        if (t_min > time_points.back())
            result_time = t_min;
    }

    return result_time;
}

// 테스트용 공 위치 시뮬레이션
cv::Vec3d getSimulatedBallPosition(double t) {
    double x = 0.3 + 2.0 * t;
    double y = 1.0;
    double z = 2.0 - 4.9 * t * t;
    if (z < 0) z = 0;
    return {x, y, z};
}

// 예측 함수
void predictBallLanding(double& out_t_hit, double& out_x_hit, double& out_y_hit, double& out_z_hit) {
    vector<double> time_points;
    vector<double> x_points, y_points, z_points;

    auto start_time = chrono::steady_clock::now();
    bool predicted = false;

    while (true) {
        double elapsed_time = chrono::duration<double>(chrono::steady_clock::now() - start_time).count();
        cv::Vec3d pos = getSimulatedBallPosition(elapsed_time);

        time_points.push_back(elapsed_time);
        x_points.push_back(pos[0]);
        y_points.push_back(pos[1]);
        z_points.push_back(pos[2]);

        if (!predicted && time_points.size() >= 3) {
            auto x_fit = quadraticFit(time_points, x_points);
            auto y_fit = quadraticFit(time_points, y_points);
            auto z_fit = quadraticFit(time_points, z_points);

            double t_hit = estimateLandingTime(z_fit, time_points);

            if (t_hit > 0) {
                out_t_hit = t_hit;
                out_x_hit = evaluateQuadratic(x_fit, t_hit);
                out_y_hit = evaluateQuadratic(y_fit, t_hit);
                out_z_hit = evaluateQuadratic(z_fit, t_hit);
                predicted = true;
            }
        }

        // 공이 지면 도달했으면 종료
        if (pos[2] <= 0 && predicted) break;

        cv::waitKey(50);
    }
}

int main() {
    double t_hit = -1;
    double x_hit = 0, y_hit = 0, z_hit = 0;

    predictBallLanding(t_hit, x_hit, y_hit, z_hit);

    cout << "예측 도착시간: " << t_hit << "초" << endl;
    cout << "예측 위치: x=" << x_hit << ", y=" << y_hit << ", z=" << z_hit << endl;

    return 0;
}