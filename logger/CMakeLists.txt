find_package(Boost REQUIRED)


add_library(logger STATIC
    Include/Logger.cpp
    Include/LogReader.cpp
)

target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${Boost_INCLUDE_DIRS}
)