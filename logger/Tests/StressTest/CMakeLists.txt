# Stress Test CMakeLists.txt

if(NOT ENABLE_MAIN)
    return()
endif()

# Create stress test executable
add_executable(logger_main Main.cpp)

target_link_libraries(logger_main PRIVATE logger)

# Link Boost if available
if(Boost_FOUND)
    target_link_libraries(logger_main PRIVATE Boost::boost)
endif()

message(STATUS "Stress test configured: logger_main")