cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/server/bin")

if (NOT UNIX)
    find_package(Boost REQUIRED COMPONENTS system thread asio)
endif ()
find_package(OpenSSL REQUIRED)

find_package(fmt CONFIG REQUIRED)

add_executable(smtp_server main.cpp
        ../networking/Connection/NetSession.cpp
        Server.cpp
        ThreadPool/src/ThreadPool.cpp
        ServerConsoleUI.cpp
        Database/Manager.cpp
)

add_custom_command(TARGET smtp_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:smtp_server>/networking/SSL/certs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/networking/SSL/certs
        $<TARGET_FILE_DIR:smtp_server>/networking/SSL/certs

        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:smtp_server>/server/config
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/server/config/config.json
        $<TARGET_FILE_DIR:smtp_server>/server/config
)

if (WIN32)
    add_custom_command(TARGET smtp_server POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/mingw64/bin/libstdc++-6.dll"
            "C:/mingw64/bin/libgcc_s_seh-1.dll"
            "C:/mingw64/bin/libwinpthread-1.dll"
            "C:/vcpkg/installed/x64-windows/bin/fmt.dll"
            $<TARGET_FILE:logger>
            $<TARGET_FILE_DIR:smtp_server>
    )
endif()

target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        networking
        ssl_support
        fmt::fmt
        OpenSSL::SSL
        OpenSSL::Crypto
        config_parser
        SMTP
        logger
        db
)

if (WIN32)
    target_link_libraries(smtp_server PRIVATE ws2_32 mswsock)
endif ()