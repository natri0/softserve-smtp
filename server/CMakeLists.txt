cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system thread asio)
find_package(OpenSSL REQUIRED)

add_executable(smtp_server main.cpp
        ../networking/Session.cpp
        Server.cpp
        ThreadPool/src/ThreadPool.cpp
        ServerConsoleUI.cpp
)

target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        ws2_32
        mswsock
        networking
        ssl_support
        OpenSSL::SSL
        OpenSSL::Crypto
)