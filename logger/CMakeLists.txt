find_package(Boost REQUIRED)

find_package(Threads REQUIRED)

add_library(logger
    Include/Logger.cpp
    Include/LogData.cpp)

target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(logger PUBLIC Threads::Threads)


add_executable(logger_main
    Tests/StressTest/Main.cpp )

add_executable(logger_example
    Example/Example.cpp )

target_link_libraries(logger_main PRIVATE logger)


target_link_libraries(logger_example PRIVATE
    logger 
    Threads::Threads 
    Boost::boost)



find_package(benchmark QUIET)
if (NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found — downloading...")
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark's own tests" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable Google Test inside benchmark" FORCE)
    set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "Disable bundled GTest in benchmark" FORCE)

    FetchContent_MakeAvailable(benchmark)
endif()



add_executable(logger_benchmark
    Tests/Benchmarks/Benchmarks.cpp)


target_include_directories(logger_benchmark
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(logger_benchmark
    PRIVATE
    logger
    benchmark::benchmark
    Threads::Threads
    Boost::boost
)


find_package(GTest QUIET)
if (NOT GTest_FOUND)
    message(STATUS "GoogleTest not found — downloading...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

add_executable(logger_tests
    Tests/UnitTests/test.cpp)

target_include_directories(logger_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(logger_tests
    PRIVATE
    logger
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    Boost::boost
)

include(GoogleTest)
gtest_discover_tests(logger_tests)

