cmake_minimum_required(VERSION 3.30)
project(softserve_smtp)

set(CMAKE_CXX_STANDARD 23)
set(BOOST_ENABLE_CMAKE_TARGETS ON)

find_package(OpenSSL REQUIRED)

add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(client)
add_subdirectory(networking)
add_subdirectory(db)
add_subdirectory(SMTP)
add_subdirectory(logger)