cmake_minimum_required(VERSION 3.15)
project(hw2)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw2 main.cpp)

find_package( OpenCV REQUIRED )
find_package( Eigen3 REQUIRED)
target_link_libraries(hw2 ${OpenCV_LIBS})
target_link_libraries (hw2 Eigen3::Eigen)
find_package( OpenMp REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${OpenMP_C_FLAGS}")
endif()
target_link_libraries(hw2 ${OpenCV_LIBS})
