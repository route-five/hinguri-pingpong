#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
using namespace cv;

Vec3d pixelToCameraRay(Point2f uv, Mat cameraMatrix) {
    double fx = cameraMatrix.at<double>(0, 0);
    double fy = cameraMatrix.at<double>(1, 1);
    double cx = cameraMatrix.at<double>(0, 2);
    double cy = cameraMatrix.at<double>(1, 2);

    double x = (uv.x - cx) / fx;
    double y = (uv.y - cy) / fy;

    Vec3d ray(x, y, 1.0);
    return normalize(ray);
}

// z=0 평면 & 교점
Point3d intersectRayWithGround(Vec3d rayDir, Vec3d camPos) {
    double t = -camPos[2] / rayDir[2];
    double x = camPos[0] + t * rayDir[0];
    double y = camPos[1] + t * rayDir[1];
    return Point3d(x, y, 0.0);
}

Vec3d transformToWorld(Vec3d camPoint, Mat R, Mat t) {
    Mat camMat = (Mat_<double>(3,1) << camPoint[0], camPoint[1], camPoint[2]);
    Mat worldMat = R.t() * (camMat - t);
    return Vec3d(worldMat);
}

// 2차 다항 회귀
vector<double> quadraticFit(const vector<double>& xs, const vector<double>& ys) {
    int n = xs.size();
    double sumX = 0, sumX2 = 0, sumX3 = 0, sumX4 = 0;
    double sumY = 0, sumXY = 0, sumX2Y = 0;

    for (int i = 0; i < n; i++) {
        double x = xs[i];
        double y = ys[i];
        sumX += x;
        sumX2 += x * x;
        sumX3 += x * x * x;
        sumX4 += x * x * x * x;

        sumY += y;
        sumXY += x * y;
        sumX2Y += x * x * y;
    }

    Mat A = (Mat_<double>(3,3) << n, sumX, sumX2,
                                  sumX, sumX2, sumX3,
                                  sumX2, sumX3, sumX4);
    Mat B = (Mat_<double>(3,1) << sumY, sumXY, sumX2Y);

    Mat coeffs;
    solve(A, B, coeffs, DECOMP_SVD);

    return {coeffs.at<double>(2), coeffs.at<double>(1), coeffs.at<double>(0)};  // a,b,c
}

double evaluateQuadratic(const vector<double>& coeffs, double x) {
    return coeffs[0]*x*x + coeffs[1]*x + coeffs[2];
}

int main() {
    // 카메라 내부 파라미터 설정 (걍 아무 값이나 넣어둔거임)
    Mat cameraMatrix = (Mat_<double>(3,3) << 800, 0, 320,
                                             0, 800, 240,
                                             0,   0,   1);

    // 카메라 외부 파라미터 설정 (이것도 걍 아무 값이나 넣어둔거임)
    Mat R = Mat::eye(3,3,CV_64F);  // 회전 matrix
    Mat t = (Mat_<double>(3,1) << 0,0,2);  // 카메라가 z=2m 위에 있다고 가정

    // 테스트용 픽셀 좌표 (측면 카메라에서 본거임) -> 걍 암거나 넣어둠
    vector<Point2f> detected_pixels = { Point2f(310,200), Point2f(315,180), Point2f(320,160) };

    vector<double> x_points, z_points;

    for (auto& uv : detected_pixels) {
        Vec3d rayDir = pixelToCameraRay(uv, cameraMatrix);
        Vec3d camPos = Vec3d(t.at<double>(0), t.at<double>(1), t.at<double>(2));
        Point3d groundPoint = intersectRayWithGround(rayDir, camPos);

        cout << "3D 지면 교점: (" << groundPoint.x << ", " << groundPoint.y << ", " << groundPoint.z << ")" << endl;

        // 좌표계 변환
        Vec3d worldPoint = transformToWorld(Vec3d(groundPoint.x, groundPoint.y, groundPoint.z), R, t);
        x_points.push_back(worldPoint[0]);
        z_points.push_back(worldPoint[2]);  // 높이
    }

    // 2차 회귀로 z(x) 궤적 근사
    auto coeffs = quadraticFit(x_points, z_points);
    cout << "회귀식: z = " << coeffs[0] << " * x^2 + " << coeffs[1] << " * x + " << coeffs[2] << endl;

    double x_test = 2.0;
    double z_test = evaluateQuadratic(coeffs, x_test);
    cout << "x=" << x_test << "일 때 예측 높이 z=" << z_test << endl;

    return 0;
}