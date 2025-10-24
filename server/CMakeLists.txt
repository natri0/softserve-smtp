cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(smtp_server ${SERVER_SOURCES})
target_link_libraries(smtp_server SMTP db)