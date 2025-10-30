cmake_minimum_required(VERSION 3.10)
project(THREADPOOL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include paths (at the beginning so CMake sees everything)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/BoostCustom/asio/include)

# Find Threads for cross-platform support (pthread on Linux/macOS, WinThreads on Windows)
find_package(Threads REQUIRED)

# Add executable for the server
add_executable(TestServer
        src/ThreadPool.cpp
)

# Settings for TestServer: optimization, Threads
target_compile_options(TestServer PRIVATE -O3)
target_link_libraries(TestServer PRIVATE Threads::Threads)

# Add executable for the client
add_executable(TestClient
    TestConnection/TestClientConnection.cpp
)

# Settings for TestClient: optimization, Threads
if(MSVC)
    target_compile_options(TestServer PRIVATE /O2)
    target_compile_options(TestClient PRIVATE /O2)
else()
    target_compile_options(TestServer PRIVATE -O3)
    target_compile_options(TestClient PRIVATE -O3)
endif()