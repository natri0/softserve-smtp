cmake_minimum_required(VERSION 3.10)

# Set compiler before project() to avoid resets
if(NOT DEFINED CMAKE_CXX_COMPILER AND EXISTS "/opt/homebrew/bin/g++-15")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-15")
    set(CMAKE_C_COMPILER "/opt/homebrew/bin/gcc-15")
endif()

project(AsyncLogger
        VERSION 1.0.0
        DESCRIPTION "Asynchronous C++20 Logger with Lock-Free Queues"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Report which compiler is being used
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC ${CMAKE_CXX_COMPILER_VERSION}: ${CMAKE_CXX_COMPILER}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(STATUS "Using Clang ${CMAKE_CXX_COMPILER_VERSION}: ${CMAKE_CXX_COMPILER}")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0")
        message(WARNING "Your Clang version may not have full C++20 support. Consider installing GCC-15 with: brew install gcc@15")
    endif()
endif()

# Find required packages
find_package(Threads REQUIRED)
find_package(Boost QUIET)

if(Boost_FOUND)
    message(STATUS "Boost found at ${Boost_INCLUDE_DIRS}")
else()
    message(STATUS "Boost not found - some features may be limited")
endif()



# Create logger library
add_library(logger SHARED
        Include/Logger.cpp
        Include/LogData.cpp)

if(WIN32)
    if(ENABLE_EXAMPLE)
        add_custom_command(TARGET logger POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:logger>
                $<TARGET_FILE_DIR:logger_example>)
    endif()

    if(ENABLE_MAIN)
        add_custom_command(TARGET logger POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:logger>
                $<TARGET_FILE_DIR:logger_main>)
    endif()
endif()


target_include_directories(logger PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include)

target_compile_features(logger PUBLIC cxx_std_20)

if(Boost_FOUND)
    target_include_directories(logger PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(logger PUBLIC Boost::boost)
endif()

target_link_libraries(logger PUBLIC Threads::Threads)

# Enable testing if requested (must be called before add_subdirectory)
if(ENABLE_TESTS)
    enable_testing()
endif()

# Add subdirectories for different components
if(ENABLE_EXAMPLE)
    add_subdirectory(Example)
endif()


add_subdirectory(Tests)


# Print build summary
message(STATUS "")
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Boost Support: ${Boost_FOUND}")
message(STATUS "Benchmarks: ${ENABLE_BENCHMARKS}")
message(STATUS "Tests: ${ENABLE_TESTS}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  - logger (library)")
if(ENABLE_EXAMPLE)
    message(STATUS "  - logger_example (basic example)")
endif()
if(ENABLE_MAIN)
    message(STATUS "  - logger_main")
endif()

if(ENABLE_BENCHMARKS)
    message(STATUS "  - logger_benchmark")
endif()
if(ENABLE_TESTS)
    message(STATUS "  - logger_tests")
endif()
message(STATUS "=====================================")
message(STATUS "")