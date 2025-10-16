cmake_minimum_required(VERSION 3.30)
project(softserve_smtp)

set(CMAKE_CXX_STANDARD 23)

add_library(Boost_headers INTERFACE)
target_include_directories(Boost_headers INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/boost_1_89_0"
)

set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/OpenSSL")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

add_library(OpenSSL::Crypto STATIC IMPORTED)
set_target_properties(OpenSSL::Crypto PROPERTIES
        IMPORTED_LOCATION "${OPENSSL_ROOT_DIR}/lib/libcrypto.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "ws2_32;gdi32;advapi32;crypt32;user32"
)

add_library(OpenSSL::SSL STATIC IMPORTED)
set_target_properties(OpenSSL::SSL PROPERTIES
        IMPORTED_LOCATION "${OPENSSL_ROOT_DIR}/lib/libssl.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
)
target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)

add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(client)
add_subdirectory(networking)
add_subdirectory(db)
add_subdirectory(SMTP)