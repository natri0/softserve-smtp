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
