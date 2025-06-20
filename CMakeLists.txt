cmake_minimum_required(VERSION 3.31)
project(hinguri_pingpong)

set(CMAKE_CXX_STANDARD 20)

add_executable(hinguri_pingpong
        src/main.cpp
)

# Boost
find_package(Boost 1.88 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(hinguri_pingpong ${Boost_LIBRARIES})

# NumCpp
add_subdirectory(lib/NumCpp)
target_link_libraries(hinguri_pingpong NumCpp::NumCpp)
