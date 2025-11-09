cmake_minimum_required(VERSION 3.30)
project(profiler)

set(CMAKE_CXX_STANDARD 23)

add_library(profiler "Profiler.cpp")

target_include_directories(profiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
