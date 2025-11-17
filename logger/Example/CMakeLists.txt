# Example CMakeLists.txt

# Create example executable
add_executable(logger_example Example.cpp)

target_link_libraries(logger_example PRIVATE 
    logger 
    Threads::Threads)

# Link Boost if available
if(Boost_FOUND)
    target_link_libraries(logger_example PRIVATE Boost::boost)
endif()

message(STATUS "Example configured: logger_example")
