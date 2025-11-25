# Unit Tests CMakeLists.txt

if(NOT ENABLE_TESTS)
    return()
endif()

message(STATUS "Configuring unit tests...")

# Download GoogleTest if not found
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test executable
add_executable(logger_tests test.cpp)

target_include_directories(logger_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/Include)

target_link_libraries(logger_tests PRIVATE
        logger
        gtest
        gtest_main
        Threads::Threads)

# Link Boost if available
if(Boost_FOUND)
    target_link_libraries(logger_tests PRIVATE Boost::boost)
endif()

# Discover tests (enable_testing() called at root level)
include(GoogleTest)
gtest_discover_tests(logger_tests)

message(STATUS "Unit tests configured: logger_tests")