cmake_minimum_required(VERSION 3.31)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 20)

add_executable(hinguri_pingpong
        src/object_detection.cpp
#        src/opencv_test.cpp
#        src/dynamixel/dynamixel.cpp
#        src/dynamixel/mx64_p2.h
#        src/dynamixel/protocol2.h
#        src/dynamixel/macros.h
#        src/dynamixel/dynamixel.h
#        src/dynamixel/mx28_p2.h
)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# Dinamixel SDK
find_package(DynamixelSDK REQUIRED)

# Library Linking
target_link_libraries(hinguri_pingpong ${OpenCV_LIBS})