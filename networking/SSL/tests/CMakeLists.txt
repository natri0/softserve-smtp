add_executable(ssl_tests
        CryptoManagerTest.cpp
        KeyExchangerTest.cpp
        SSLContextFactoryTest.cpp
)

target_link_libraries(ssl_tests
        PRIVATE
        SSL
        GTest::gtest_main
)

file(COPY
        "${CMAKE_SOURCE_DIR}/3rdparty/OpenSSL/bin/libssl-3-x64.dll"
        "${CMAKE_SOURCE_DIR}/3rdparty/OpenSSL/bin/libcrypto-3-x64.dll"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

include(GoogleTest)
gtest_discover_tests(ssl_tests)