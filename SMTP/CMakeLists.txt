cmake_minimum_required(VERSION 3.30)
project(SMTP VERSION 0.1 DESCRIPTION "SMTP implementation")

set(CMAKE_CXX_STANDARD 23)

add_library(SMTP 
	"source/smtp.cpp")
target_include_directories(SMTP PUBLIC "include/")
