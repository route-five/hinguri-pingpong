//
// Created by Hyunseung Ryu on 2025. 8. 1..
//

#include <opencv2/opencv.hpp>

int main() {
    for (int i = 0; i <= 16; ++i) {
        std::ostringstream filename;
        filename << std::setw(2) << std::setfill('0') << i << ".png";

        const cv::Mat img = cv::imread("img/chessboard/right_og/" + filename.str()); // 원본 이미지 읽기
        if (img.empty()) return -1;

        cv::Mat resized;
        cv::resize(img, resized, cv::Size(), 0.5, 0.5); // x, y 방향으로 0.5배 축소

        if (cv::imwrite("img/chessboard/right/" + filename.str(), resized))
            std::cout << filename.str() << " - Image resized and saved successfully." << std::endl;
        else
            std::cout << filename.str() << " - Failed to save the resized image." << std::endl;
    }

    return 0;
}
