find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(ssl_tests
        CryptoManagerTest.cpp
        KeyExchangerTest.cpp
        SSLContextFactoryTest.cpp
)

target_link_libraries(ssl_tests
        PRIVATE
        ssl_support
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ssl_tests)