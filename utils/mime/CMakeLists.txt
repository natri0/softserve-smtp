cmake_minimum_required(VERSION 3.30)
project(utils)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../3rdparty/boost_1_89_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

include(FetchContent)

set(BUILD_GMOCK ON CACHE BOOL "Build GoogleMock with GoogleTest" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Don't install GTest" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(FETCHCONTENT_QUIET FALSE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(TestMime
    test/TestMime.cpp
    source/EmailMessage.cpp
    source/MimeFormatter.cpp
    source/MimeParser.cpp
    source/EmailBuilder.cpp
    ../../networking/SSL/CryptoManager.cpp
    ../../networking/SSL/SSLContextFactory.cpp
    ../../networking/SSL/KeyExchanger.cpp
    
)


target_include_directories(TestMime PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}                      
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/.. 
    ${CMAKE_SOURCE_DIR}/../../networking
    ${CMAKE_SOURCE_DIR}/../../networking/SSL             
)

if(WIN32)
    target_link_libraries(TestMime PRIVATE
        ${Boost_LIBRARIES}
        ws2_32
        mswsock
        OpenSSL::SSL
        OpenSSL::Crypto
        GTest::gmock_main
    )
else() # Linux / macOS
    target_link_libraries(TestMime PRIVATE
        ${Boost_LIBRARIES}
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
        GTest::gmock_main
    )
endif()
include(GoogleTest)
gtest_discover_tests(TestMime)
