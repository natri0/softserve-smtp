if (NOT UNIX)
    find_package(Boost REQUIRED COMPONENTS system thread asio)
endif ()

find_package(OpenSSL REQUIRED)

add_library(networking STATIC
        Session.cpp
        SmartSession.cpp
)

target_include_directories(networking
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${boost_INLCUDE_DIRS}
)

target_link_libraries(networking
        PUBLIC
        ${Boost_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        SMTP
        logger
        ssl_support
)

if (WIN32)
    target_link_libraries(networking PRIVATE ws2_32 mswsock)
endif ()