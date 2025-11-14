cmake_minimum_required(VERSION 3.30)

find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(auth_tests
        AuthenticationTest.cpp
)

target_link_libraries(auth_tests
        PRIVATE
        SMTP
        GTest::gtest_main
        utils
)

if(WIN32)
    set(RUNTIME_DLL_DIRS
            "C:/Users/mmmal/.vcpkg-clion/vcpkg/installed/x64-windows/debug/bin"
            "C:/Program Files/OpenSSL/bin"
    )

    foreach(dir IN LISTS RUNTIME_DLL_DIRS)
        if(EXISTS "${dir}")
            file(GLOB dlls "${dir}/*.dll")
            foreach(dll IN LISTS dlls)
                add_custom_command(TARGET auth_tests POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:auth_tests>
                )
            endforeach()
        endif()
    endforeach()
endif()

include(GoogleTest)
gtest_add_tests(TARGET auth_tests SOURCES AuthenticationTest.cpp)