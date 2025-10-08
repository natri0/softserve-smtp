cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(smtp_server main.cpp
        config/configreader.h)

add_executable(json_test config/json.cpp config/formatutil/optional.h config/formatutil/variant.h
        config/json.h
        config/formatutil/jsonvalue.h)
