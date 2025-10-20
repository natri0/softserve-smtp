add_library(SSL
        CryptoManager.cpp
        KeyExchanger.cpp
        SSLContextFactory.cpp
)

target_include_directories(SSL PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SSL PUBLIC
        Boost_headers
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_compile_definitions(SSL PUBLIC
        SSL_CERT_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/server.crt"
        SSL_KEY_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/server.key"
        SSL_DH_PARAMS_FILE="${CMAKE_SOURCE_DIR}/networking/SSL/certs/dh.pem"
)

add_subdirectory(tests)