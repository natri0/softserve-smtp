cmake_minimum_required(VERSION 3.30)
project(auth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(auth
        src/AuthMethodFactory.cpp
        src/LoginAuth.cpp
        src/PlainAuth.cpp
)