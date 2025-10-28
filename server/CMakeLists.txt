cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(smtp_server main.cpp)
target_link_libraries(smtp_server PRIVATE config_parser)