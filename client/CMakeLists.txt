cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "C:/Users/alkir/boost_1_89_0/boost_1_89_0")

add_executable(smtp_client main.cpp

        ../networking/Session.cpp
        Client.cpp
        EmailMessage.cpp
        EmailMessage.h
)

target_include_directories(smtp_client PRIVATE ${BOOST_ROOT})
target_link_libraries(smtp_client PRIVATE ws2_32 mswsock networking)