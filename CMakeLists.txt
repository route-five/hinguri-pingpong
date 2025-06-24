cmake_minimum_required(VERSION 3.31)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 20)

add_executable(hinguri_pingpong
        src/2d_predictor.cpp
        src/random/Random.hpp
)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(hinguri_pingpong ${OpenCV_LIBS})