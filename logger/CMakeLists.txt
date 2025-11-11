find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_library(logger STATIC
    Include/Logger.cpp)

target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${Boost_INCLUDE_DIRS}
)
