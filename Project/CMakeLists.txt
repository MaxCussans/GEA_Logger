project(GameAnalytics)
cmake_minimum_required(VERSION 2.8.12)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_executable(GameAnalyticsTool main.cpp
src/Heatmaps.cpp src/Heatmaps.h
src/Parsing.cpp src/Parsing.h)
target_link_libraries(GameAnalyticsTool ${CONAN_LIBS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameAnalyticsTool")