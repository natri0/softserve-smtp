cmake_minimum_required(VERSION 3.28)
project(ssl_support LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_library(ssl_support STATIC
        KeyExchanger.cpp
        SSLContextFactory.cpp
        CryptoManager.cpp
)

target_include_directories(ssl_support PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(ssl_support PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        ${Boost_LIBRARIES}
)

if (WIN32)
    target_link_libraries(ssl_support PUBLIC ws2_32 mswsock)
endif ()