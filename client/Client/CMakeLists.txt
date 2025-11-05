cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/client/bin")

find_package(Boost REQUIRED COMPONENTS system thread asio)
find_package(OpenSSL REQUIRED)

add_library(smtp_client_lib
    Client.cpp
    EmailMessage.cpp
)

add_custom_command(TARGET smtp_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:smtp_client>/networking/SSL/certs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/networking/SSL/certs
        $<TARGET_FILE_DIR:smtp_client>/networking/SSL/certs
)

target_include_directories(smtp_client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(smtp_client_lib PUBLIC
        ${Boost_LIBRARIES}
        mswsock
        networking
        ssl_support
	Boost::system
        Boost::thread
        Boost::asio
        OpenSSL::SSL
        OpenSSL::Crypto

        ${PLATFORM_LIBS}
)
