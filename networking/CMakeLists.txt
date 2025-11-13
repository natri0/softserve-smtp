if (NOT UNIX)
    find_package(Boost REQUIRED COMPONENTS system thread asio)
endif ()
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
        OpenSSL::SSL
        OpenSSL::Crypto
        SMTP
)

if (WIN32)
    target_link_libraries(networking PRIVATE ws2_32 mswsock)
endif ()

add_subdirectory(SSL)