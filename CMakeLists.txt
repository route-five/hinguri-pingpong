cmake_minimum_required(VERSION 3.28)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 20)

add_executable(hinguri_pingpong
#        src/opencv_test.cpp
#        src/object_detection.cpp
        src/quadratic.cpp
)

#set(OpenCV_DIR "C:/OpenCV/x64/mingw/lib")
#set(OpenCV_INCLUDE_DIRS "C:/OpenCV")
#set(OpenCV_LIB_DIR "/usr/lib64")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(hinguri_pingpong ${OpenCV_LIBS})