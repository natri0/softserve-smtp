cmake_minimum_required(VERSION 3.30)
project(gtests DESCRIPTION "Google Tests")
set(CMAKE_CXX_STANDARD 23)

#get googletest
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/CPM.cmake)
if (NOT TARGET GTest::GTest)
    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.17.0
        OPTIONS
            "BUILD_GMOCK OFF"
            "INSTALL_GTEST OFF"
    )
endif()

add_executable(SMTP_tests "gtests.cpp" "SMTPConfigBuilderTests.h")
target_link_libraries(SMTP_tests SMTP GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(SMTP_tests)

