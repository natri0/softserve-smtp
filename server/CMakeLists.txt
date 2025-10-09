cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "C:/Users/alkir/boost_1_89_0/boost_1_89_0")

add_executable(smtp_server main.cpp
        ../networking/Session.cpp
        ../networking/Session.h
        Server.cpp
        Server.h
        ../client/Client.cpp
        ../client/Client.h
        ThreadPool.cpp
        ThreadPool.h
)

target_include_directories(smtp_server PRIVATE ${BOOST_ROOT})
target_link_libraries(smtp_server PRIVATE ws2_32 mswsock)
