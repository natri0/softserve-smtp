cmake_minimum_required(VERSION 3.30)
project(utils)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_library(utils
        Base64.cpp
        Base64.h
)

target_link_libraries(utils PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)