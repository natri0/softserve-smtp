cmake_minimum_required(VERSION 3.30)

project(THREADPOOL LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)


set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../../3rdparty/boost_1_89_0")

set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system thread)

find_package(OpenSSL REQUIRED)


# 🔹 Include paths
# include_directories(
#     ${CMAKE_SOURCE_DIR}/include
#     ${CMAKE_SOURCE_DIR}/BoostCustom/asio/include
#     ${CMAKE_SOURCE_DIR}/includeForTest
# )

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}${CMAKE_SOURCE_DIR}/../../3rdparty/boost_1_89_0/boost/asio.hpp
    ${CMAKE_SOURCE_DIR}/../../3rdparty/boost_1_89_0
    ${CMAKE_SOURCE_DIR}/includeForTest
    ${CMAKE_SOURCE_DIR}/include
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

target_link_libraries(TestServer 
    PRIVATE 
        Threads::Threads
        Boost::system
        Boost::thread
)

# =====================================================================
# 🔸 Executable: TestClient
# =====================================================================
add_executable(TestClient
    TestConnection/TestClientConnection.cpp
)

target_link_libraries(TestClient 
    PRIVATE 
        Threads::Threads
        Boost::system
        Boost::thread
)

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

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(ThreadPoolIntegrationTest
    TestConnection/ThreadPoolIntegrationTest.cpp
    src/ThreadPool.cpp
)

target_link_libraries(ThreadPoolIntegrationTest
    PRIVATE
        Boost::system
        Boost::thread
        Threads::Threads
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(ThreadPoolIntegrationTest)

# ====== SERVER ======
add_executable(smtp_server
    ../main.cpp
    ../../networking/Session.cpp
    ../../networking/SSL/CryptoManager.cpp
    ../../networking/SSL/SSLContextFactory.cpp
    ../../networking/SSL/KeyExchanger.cpp
    ../Server.cpp
    src/ThreadPool.cpp
    ../ServerConsoleUI.cpp
)

# ====== CLIENT ======
add_executable(smtp_client 
    ../../client/main.cpp
    ../../networking/Session.cpp
    ../../networking/SSL/CryptoManager.cpp
    ../../networking/SSL/SSLContextFactory.cpp
    ../../networking/SSL/KeyExchanger.cpp
    ../../client/Client.cpp
    ../../client/EmailMessage.cpp
)

# ====== INCLUDE DIRECTORIES ======
set(COMMON_INCLUDES
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../../networking
    ${CMAKE_SOURCE_DIR}/../../networking/SSL
)

target_include_directories(smtp_server PRIVATE ${COMMON_INCLUDES})
target_include_directories(smtp_client PRIVATE ${COMMON_INCLUDES})

# ====== LINKING ======
if(WIN32)
    target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        ws2_32 mswsock
        OpenSSL::SSL OpenSSL::Crypto
    )
    target_link_libraries(smtp_client PRIVATE
        ${Boost_LIBRARIES}
        ws2_32 mswsock
        OpenSSL::SSL OpenSSL::Crypto
    )
else()
    target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        pthread
        OpenSSL::SSL OpenSSL::Crypto
    )
    target_link_libraries(smtp_client PRIVATE
        ${Boost_LIBRARIES}
        pthread
        OpenSSL::SSL OpenSSL::Crypto
    )
endif()

