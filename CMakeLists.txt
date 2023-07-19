cmake_minimum_required(VERSION 3.25)
project(Triangular_stereo_model)

set(CMAKE_CXX_STANDARD 17)

#只关心源文件
add_library(lib src/Build.cpp src/Camera.cpp src/Phase.cpp src/Project.cpp src/utils.cpp)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(Triangular_stereo_model main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} lib)
