cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)

add_executable(smtp_server main.cpp)

add_executable(json_test
        config/json.cpp
        config/formatutil/optional.h
        config/formatutil/variant.h
        config/json.h
        config/formatutil/jsonvalue.h
        config/json_test.cpp)

target_link_libraries(json_test PRIVATE GTest::gtest_main)
