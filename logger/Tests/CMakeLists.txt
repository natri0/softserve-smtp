# Tests subdirectory CMakeLists.txt

# Tests directory coordinator

# Add subdirectories for different test types
add_subdirectory(UnitTests)
add_subdirectory(Benchmarks)

if(ENABLE_MAIN)
    add_subdirectory(StressTest)
endif()