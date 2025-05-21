cmake_minimum_required(VERSION 3.5)
project(fractal_viewer)

set(EIGEN3_INCLUDE_DIR "/usr/include/eigen3")
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/third_party/nanovg/src)

#add_subdirectory(nanogui)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(fractal_viewer src/main.cpp src/glad.c)
target_link_libraries(fractal_viewer PRIVATE nanogui)
target_include_directories(fractal_viewer PRIVATE nanogui/include)