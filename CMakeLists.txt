cmake_minimum_required(VERSION 3.28)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 23)

add_executable(hinguri_pingpong
        src/vision/pre/points/get_camera_points.cpp
        src/vision/predictor.h
        src/vision/calibration.h
)

#set(OpenCV_DIR "C:/OpenCV/x64/mingw/lib")
#set(OpenCV_INCLUDE_DIRS "C:/OpenCV")
#set(OpenCV_LIB_DIR "/usr/lib64")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# Dynamixel SDK
#find_package(DynamixelSDK REQUIRED)

# Linear Motor
#find_package(LinearMotor REQUIRED)

# Matplot++
find_package(Matplot++ REQUIRED)
target_link_libraries(hinguri_pingpong Matplot++::matplot)

# Library Linking
target_link_libraries(hinguri_pingpong
        ${OpenCV_LIBS}
        #        dynamixel_sdk::dynamixel_sdk
        #        linear_motor::linear_motor
)