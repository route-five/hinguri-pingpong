cmake_minimum_required(VERSION 3.28)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 23)

add_executable(hinguri_pingpong
        test/opencv/tracking/moment_kalman.cpp
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

# Library Linking
target_link_libraries(hinguri_pingpong
        ${OpenCV_LIBS}
        #        dynamixel_sdk::dynamixel_sdk
        #        linear_motor::linear_motor
)