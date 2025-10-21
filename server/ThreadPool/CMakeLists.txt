cmake_minimum_required(VERSION 3.14)
project(THREADPOOL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔹 Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/BoostCustom/asio/include
    ${CMAKE_SOURCE_DIR}/includeForTest
)

# 🔹 Find Threads (cross-platform)
find_package(Threads REQUIRED)

# =====================================================================
# 🔸 Executable: TestServer
# =====================================================================
add_executable(TestServer
    TestConnection/ServerFromTestClient.cpp
    TestConnection/SMTPSessionForTest.cpp
    src/ThreadPool.cpp
)

target_link_libraries(TestServer PRIVATE Threads::Threads)

# =====================================================================
# 🔸 Executable: TestClient
# =====================================================================
add_executable(TestClient
    TestConnection/TestClientConnection.cpp
)

target_link_libraries(TestClient PRIVATE Threads::Threads)

# =====================================================================
# 🔸 GoogleTest + GoogleMock (FetchContent)
# =====================================================================
include(FetchContent)

set(BUILD_GMOCK ON CACHE BOOL "Build GoogleMock with GoogleTest" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Don't install GTest" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(FETCHCONTENT_QUIET FALSE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# =====================================================================
# 🔸 Executable: ThreadPoolIntegrationTest
# =====================================================================
add_executable(ThreadPoolIntegrationTest
    TestConnection/ThreadPoolIntegrationTest.cpp
    src/ThreadPool.cpp
)

target_link_libraries(ThreadPoolIntegrationTest
    PRIVATE
        Threads::Threads
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(ThreadPoolIntegrationTest)

# =====================================================================
# 🔸 Optimization flags
# =====================================================================
if(MSVC)
    target_compile_options(TestServer PRIVATE /O2)
    target_compile_options(TestClient PRIVATE /O2)
    target_compile_options(ThreadPoolIntegrationTest PRIVATE /O2)
else()
    target_compile_options(TestServer PRIVATE -O3)
    target_compile_options(TestClient PRIVATE -O3)
    target_compile_options(ThreadPoolIntegrationTest PRIVATE -O3)
endif()
