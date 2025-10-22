include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip)

FetchContent_MakeAvailable(googletest)
find_package(OpenSSL REQUIRED)

add_executable(SessionTest SessionTest.cpp)

target_link_libraries(SessionTest PRIVATE
        gtest_main
        networking
        ssl_support
        OpenSSL::SSL
        OpenSSL::Crypto)

target_include_directories(SessionTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/networking
        ${gtest_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(SessionTest)