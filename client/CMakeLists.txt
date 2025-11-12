cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/client/bin")

if (NOT UNIX)
    find_package(Boost REQUIRED COMPONENTS system thread asio)
endif ()
find_package(OpenSSL REQUIRED)

add_executable(smtp_client main.cpp
        ../networking/Connection/NetSession.cpp
        Client.cpp
        EmailMessage.cpp
)

add_custom_command(TARGET smtp_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:smtp_client>/networking/SSL/certs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/networking/SSL/certs
        $<TARGET_FILE_DIR:smtp_client>/networking/SSL/certs
)

target_include_directories(smtp_client PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(smtp_client PRIVATE
        ${Boost_LIBRARIES}
        networking
        ssl_support
        OpenSSL::SSL
        OpenSSL::Crypto
)

if (WIN32)
    target_link_libraries(smtp_client PRIVATE ws2_32 mswsock)
endif ()