cmake_minimum_required(VERSION 3.28)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 23)

add_executable(hinguri_pingpong
        src/simulation/simulator_side.hpp
        src/simulation/simulator_top.hpp
        src/utils/draw.hpp
        src/utils/constants.hpp
        src/vision/calibrator.hpp
        src/vision/camera.hpp
        src/vision/camera_type.hpp
        src/vision/contour.hpp
        src/vision/predictor.hpp
        src/vision/tracker.hpp
        src/vision/bridge.hpp
        src/vision/visualizer.hpp
        src/vision/vision_end.hpp
        src/control/dynamixel_actuator.hpp
        src/control/linear_actuator.hpp
        src/control/control_end.hpp

        #### preprocessing
        # 1. 탁구대 위의 World 좌표를 생성하는 코드
        #        src/vision/pre/world_points.cpp

        # 2. 체커보드 이미지를 수집하는 코드
        #        src/vision/pre/chessboard_images.cpp

        # 3. 탁구대 위의 Camera 좌표를 수집하는 코드
        #        src/vision/pre/camera_points.cpp

        # 4. 모든 generate된 데이터를 이용하여 카메라를 보정하는 코드
        #        src/vision/pre/calibrate.cpp

        #### processing
        #        src/main.cpp
        #        src/vision.cpp
        src/main.cpp

        #### test
        #        test/matplot++/bridge_convert_test.cpp
        #        test/matplot++/visualize_pingpong.cpp
        #        test/opencv/top_camera_test.cpp
        #        test/opencv/get_world_pos.cpp
        #        test/opencv/triple_camera.cpp
        #        test/matplot++/predict_world_pos_test.cpp
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