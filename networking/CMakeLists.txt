find_package(Boost REQUIRED COMPONENTS system thread asio)
find_package(OpenSSL REQUIRED)

add_library(networking
        Session.cpp
        SmartSession.cpp
        SmartSession.h
)

target_include_directories(networking
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${boost_INLCUDE_DIRS}
)

target_link_libraries(networking
        PUBLIC
        ${Boost_LIBRARIES}
        ws2_32
        mswsock
        OpenSSL::SSL
        OpenSSL::Crypto
        SMTP
        logger
)