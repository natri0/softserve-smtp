cmake_minimum_required(VERSION 3.30)
project(ssl_support LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_library(ssl_support
        src/KeyExchanger.cpp
        src/SSLContextFactory.cpp
        src/CryptoManager.cpp
)

target_include_directories(ssl_support PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(ssl_support PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)