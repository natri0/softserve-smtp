# Benchmarks CMakeLists.txt

if(NOT ENABLE_BENCHMARKS)
    return()
endif()

message(STATUS "Configuring benchmarks...")

# Download Google Benchmark if not found
include(FetchContent)
FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark's own tests" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable Google Test inside benchmark" FORCE)

FetchContent_MakeAvailable(benchmark)

# Create benchmark executable
add_executable(logger_benchmark Benchmarks.cpp)

target_include_directories(logger_benchmark PRIVATE
        ${CMAKE_SOURCE_DIR}/Include)

target_link_libraries(logger_benchmark PRIVATE
        logger
        benchmark::benchmark
        Threads::Threads)

# Link Boost if available
if(Boost_FOUND)
    target_link_libraries(logger_benchmark PRIVATE Boost::boost)
endif()

message(STATUS "Benchmarks configured: logger_benchmark")
