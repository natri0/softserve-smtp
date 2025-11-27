cmake_minimum_required(VERSION 3.28)
project(softserve_smtp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/std:c++latest /permissive- /Zc:__cplusplus)
    message(STATUS "MSVC Version: ${MSVC_VERSION}")
    message(STATUS "MSVC Toolset: ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Visual Studio Version: ${CMAKE_VS_VERSION_RANGE}")
endif()

set(BOOST_ENABLE_CMAKE_TARGETS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
find_package(Boost REQUIRED COMPONENTS system thread asio)
find_package(OpenSSL REQUIRED)

add_subdirectory(logger)
add_subdirectory(server)
add_subdirectory(server/config)
add_subdirectory(utils)
add_subdirectory(client)
add_subdirectory(networking)
add_subdirectory(networking/SSL)
add_subdirectory(db)
add_subdirectory(SMTP)