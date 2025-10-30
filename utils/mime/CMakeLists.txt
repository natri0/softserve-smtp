cmake_minimum_required(VERSION 3.30)
project(utils)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../3rdparty/boost_1_89_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

add_executable(test
    test/test.cpp
    ../../networking/SSL/CryptoManager.cpp
    ../../networking/SSL/SSLContextFactory.cpp
    ../../networking/SSL/KeyExchanger.cpp
    
)

# Тепер додаємо include directories
target_include_directories(test PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}                      
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/.. 
    ${CMAKE_SOURCE_DIR}/../../networking
    ${CMAKE_SOURCE_DIR}/../../networking/SSL             
)

if(WIN32)
    target_link_libraries(test PRIVATE
        ${Boost_LIBRARIES}
        ws2_32
        mswsock
        OpenSSL::SSL
        OpenSSL::Crypto
    )
else() # Linux / macOS
    target_link_libraries(test PRIVATE
        ${Boost_LIBRARIES}
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()