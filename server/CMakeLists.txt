cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(smtp_server main.cpp
        ../networking/Session.cpp
        Server.cpp
        ThreadPool/src/ThreadPool.cpp
        ServerConsoleUI.cpp
        ServerConsoleUI.h
)

target_link_libraries(smtp_server PRIVATE ws2_32 mswsock networking)