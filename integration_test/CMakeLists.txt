cmake_minimum_required(VERSION 3.30)
project(integration_test)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "C:/Users/alkir/boost_1_89_0/boost_1_89_0")

add_executable(integration_test main.cpp
        ../server/ThreadPool/src/ThreadPool.cpp
        ../networking/Session.cpp
        ../client/Client.cpp
        ../server/Server.cpp
)

target_include_directories(integration_test PRIVATE ${BOOST_ROOT})
target_link_libraries(integration_test PRIVATE ws2_32 mswsock)