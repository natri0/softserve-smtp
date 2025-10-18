cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)

add_library(config_parser STATIC
        config/json.cpp
        config/json.h
        config/config.cpp
        config/config.h)

add_executable(smtp_server main.cpp)
target_link_libraries(smtp_server PRIVATE config_parser)

add_executable(json_test
        config/formatutil/optional.h
        config/formatutil/variant.h
        config/formatutil/jsonvalue.h
        config/json_test.cpp)

target_link_libraries(json_test PRIVATE GTest::gtest_main config_parser)
