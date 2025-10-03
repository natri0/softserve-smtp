cmake_minimum_required(VERSION 3.30)
project(SMTPTest VERSION 0.1 DESCRIPTION "Project for testting SMTP")

#THIS PROJECT IS TEMPORERY AND WILL BE REMOVED ON MERGE

set(CMAKE_CXX_STANDARD 23)

add_executable(SMTPTest main.cpp)

target_link_libraries(SMTPTest PRIVATE SMTP)
