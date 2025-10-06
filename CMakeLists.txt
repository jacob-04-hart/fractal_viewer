cmake_minimum_required(VERSION 3.5)
project(fractal_viewer)

set(EIGEN3_INCLUDE_DIR "/usr/include/eigen3")
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/third_party/nanovg/src)

#add_subdirectory(nanogui)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
find_package(glfw3 3.4 REQUIRED PATHS /usr/local/lib/cmake/glfw3 NO_DEFAULT_PATH)
# find_package(glfw3 3.3 REQUIRED PATHS /local/lib/cmake/glfw3 NO_DEFAULT_PATH)
# message(STATUS "GLFW3_FOUND: ${glfw3_FOUND}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
add_executable(fractal_viewer src/main.cpp src/glad.c)
target_link_libraries(fractal_viewer PRIVATE nanogui /usr/local/lib/libglfw.so)
target_include_directories(fractal_viewer PRIVATE nanogui/include PRIVATE ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/include)

target_sources(fractal_viewer PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/nfd_common.c
    ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/nfd_zenity.c
)
if(UNIX)
    target_link_libraries(fractal_viewer PRIVATE dl)
endif()