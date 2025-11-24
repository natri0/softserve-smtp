cmake_minimum_required(VERSION 3.30)
project(db)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/server/bin")

option(DB_ENABLE_TESTS "Enable building of database module tests" OFF)
option(BUILD_SHARED_LIBS "Build libraries as shared libraries" OFF)

file(GLOB_RECURSE DB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
if (BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${DB_SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC ${DB_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CPM.cmake)
if (NOT TARGET fmt::fmt)
    CPMAddPackage("gh:fmtlib/fmt#12.0.0")
endif()

set(SQLITE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/sqlite)
target_sources(${PROJECT_NAME} PRIVATE ${SQLITE_DIR}/sqlite3.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${SQLITE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC utils fmt::fmt)