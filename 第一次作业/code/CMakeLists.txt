cmake_minimum_required(VERSION 3.15)
project(hw1)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw1 main.cpp)

find_package( OpenCV REQUIRED )
target_link_libraries(hw1 ${OpenCV_LIBS})