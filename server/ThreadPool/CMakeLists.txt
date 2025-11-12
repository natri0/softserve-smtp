cmake_minimum_required(VERSION 3.30)
project(THREADPOOL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

# =====================================================================
# 🔹 Boost build (локально)
# =====================================================================
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../../3rdparty/boost_1_89_0")

if(WIN32)
    set(BOOST_BOOTSTRAP_CMD bootstrap.bat)
    set(BOOST_BUILD_CMD b2.exe threading=multi)
else()
    execute_process(COMMAND chmod +x ${BOOST_ROOT}/bootstrap.sh)
    execute_process(COMMAND chmod +x ${BOOST_ROOT}/tools/build/src/engine/build.sh)
    set(BOOST_BOOTSTRAP_CMD bash bootstrap.sh)
    set(BOOST_BUILD_CMD ./b2 threading=multi)
endif()

ExternalProject_Add(
    Boost_Build
    SOURCE_DIR ${BOOST_ROOT}
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_CMD}
    BUILD_COMMAND ${BOOST_BUILD_CMD} --with-system --with-thread
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)

set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

# =====================================================================
# 🔹 Include paths
# =====================================================================
include_directories(
    ${BOOST_INCLUDEDIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/includeForTest
    ${CMAKE_SOURCE_DIR}/include
)

# 🔹 Find Threads (cross-platform)
find_package(Threads REQUIRED)

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
# 🔸 Executable: TestServer
# =====================================================================
add_executable(TestServer
    TestConnection/ServerFromTestClient.cpp
    TestConnection/SMTPSessionForTest.cpp
    src/ThreadPool.cpp
)

add_dependencies(TestServer Boost_Build)

# Лінкування Boost бібліотек
if(WIN32)
    file(GLOB BOOST_LIBS "${BOOST_LIBRARYDIR}/*.lib")
else()
    file(GLOB BOOST_LIBS "${BOOST_LIBRARYDIR}/*.a")
endif()

target_link_libraries(TestServer 
    PRIVATE 
        Threads::Threads
        ${BOOST_LIBS}
)

# =====================================================================
# 🔸 Executable: TestClient
# =====================================================================
add_executable(TestClient
    TestConnection/TestClientConnection.cpp
)

add_dependencies(TestClient Boost_Build)

target_link_libraries(TestClient 
    PRIVATE 
        Threads::Threads
        ${BOOST_LIBS}
)

# target_compile_definitions(TestClient PRIVATE TEST_DEBUG)
# =====================================================================
# 🔸 Executable: ThreadPoolIntegrationTest
# =====================================================================
add_executable(ThreadPoolIntegrationTest
    TestConnection/ThreadPoolIntegrationTest.cpp
    src/ThreadPool.cpp
)

add_dependencies(ThreadPoolIntegrationTest Boost_Build)

target_link_libraries(ThreadPoolIntegrationTest
    PRIVATE
        ${BOOST_LIBS}
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
