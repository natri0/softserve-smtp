cmake_minimum_required(VERSION 3.30)
project(gtests DESCRIPTION "Google Tests")
set(CMAKE_CXX_STANDARD 23)

#get googletest
include(FetchContent)
option(BUILD_GMOCK OFF)
option(INSTALL_GTEST OFF)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(gtests_smtp "gtests.cpp")

target_link_libraries(gtests_smtp SMTP gtest_main)

