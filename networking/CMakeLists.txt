set(BOOST_ROOT "C:/Users/alkir/boost_1_89_0/boost_1_89_0")

set(OPENSSL_ROOT_DIR "C:/Users/alkir/CLionProjects/softserve-smtp/3rdparty/OpenSSL")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib")

find_package(OpenSSL REQUIRED)

if (OpenSSL_FOUND)
    message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libs: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()

add_library(networking
        Session.cpp
)

target_include_directories(networking
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${BOOST_ROOT}
        ${OPENSSL_INCLUDE_DIR}
)

link_directories("${BOOST_ROOT}/stage/lib")

target_link_libraries(networking
        PUBLIC
        ws2_32
        mswsock
        "${OPENSSL_LIB_DIR}/libssl.lib"
        "${OPENSSL_LIB_DIR}/libcrypto.lib"
)