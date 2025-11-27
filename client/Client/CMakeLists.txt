cmake_minimum_required(VERSION 3.28)
project(smtp_client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/client/bin")

find_package(Boost REQUIRED COMPONENTS system thread asio uuid)
find_package(OpenSSL REQUIRED)

add_library(smtp_client_lib
    Client.cpp
    #EmailMessage.cpp
    ClientSettings.h
    Client.h
)
add_subdirectory(../../utils/mime mime_build)

add_custom_command(TARGET smtp_client_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:smtp_client_lib>/networking/SSL/certs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/networking/SSL/certs
        $<TARGET_FILE_DIR:smtp_client_lib>/networking/SSL/certs
)

target_include_directories(smtp_client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(smtp_client_lib PUBLIC
        ${Boost_LIBRARIES}
        mswsock
        networking
        mime
        ssl_support
	Boost::system
        Boost::thread
        Boost::asio
        Boost::uuid
        OpenSSL::SSL
        OpenSSL::Crypto

        ${PLATFORM_LIBS}
)
