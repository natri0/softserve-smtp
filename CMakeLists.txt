cmake_minimum_required(VERSION 3.30)
project(softserve_smtp)

set(CMAKE_CXX_STANDARD 23)
set(BOOST_ENABLE_CMAKE_TARGETS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
find_package(Boost REQUIRED COMPONENTS system thread asio)
find_package(OpenSSL REQUIRED)

add_subdirectory(server)
add_subdirectory(server/config)
add_subdirectory(utils)
add_subdirectory(client)
add_subdirectory(networking)
add_subdirectory(networking/SSL)
add_subdirectory(db)
add_subdirectory(SMTP)