cmake_minimum_required(VERSION 3.30)
project(softserve_smtp)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(client)
add_subdirectory(networking)
add_subdirectory(db)
add_subdirectory(SMTP)
