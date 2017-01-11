project(AnalyticsSolution)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(GameAnalytics main.cpp
	src/Heatmap.cpp src/Heatmap.h
	src/Parse.cpp src/Parse.h)

target_link_libraries(GameAnalytics ${CONAN_LIBS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "GameAnalytics")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameAnalytics")