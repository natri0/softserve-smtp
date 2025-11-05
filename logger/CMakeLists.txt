find_package(Boost REQUIRED)


add_executable(logger
    #Include/Logger.cpp
    #Include/LogReader.cpp
    Tests/StressTest/Main.cpp
)

target_include_directories(logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${Boost_INCLUDE_DIRS}
)