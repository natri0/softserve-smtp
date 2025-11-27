cmake_minimum_required(VERSION 3.5...3.30)
project(ssl_benchmark)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED asio)

include(FetchContent)
FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(ssl_benchmark
        CryptoManagerBenchmark.cpp
        KeyExchangerBenchmark.cpp
        ../src/CryptoManager.cpp
        ../src/KeyExchanger.cpp
        ../../../utils/Base64.cpp
)

target_include_directories(ssl_benchmark PRIVATE
        ../include
        ../../utils
)

target_link_libraries(ssl_benchmark
        PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::asio
)