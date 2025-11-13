cmake_minimum_required(VERSION 3.30)
project(ssl_support LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio)

add_library(ssl_support
        src/KeyExchanger.cpp
        src/SSLContextFactory.cpp
        src/CryptoManager.cpp
)

target_compile_definitions(ssl_support PUBLIC
        SSL_CERT_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/server.crt"
        SSL_KEY_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/server.key"
        SSL_DH_PARAMS_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/dh.pem"
)

target_include_directories(ssl_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ssl_support PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::asio
        utils
)