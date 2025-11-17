find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_library(logger STATIC
    Include/Logger.cpp
    Include/LogData.cpp)


target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(logger PUBLIC Threads::Threads)
