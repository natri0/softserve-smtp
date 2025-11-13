cmake_minimum_required(VERSION 3.30)

if(NOT DEFINED Qt6_DIR AND DEFINED ENV{VCPKG_ROOT})
    set(Qt6_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/share/Qt6")
endif()

project(smtp_client)

set(PROJECT_SOURCES
    src/main.cpp
    src/SMTPClientWindow.cpp
    src/SMTPController.cpp
    src/SMTPWorker.cpp
    include/SMTPClientWindow.h
    include/SMTPController.h
    include/SMTPWorker.h
    include/Structures.h
)

qt_add_resources(PROJECT_RESOURCES "resources.qrc")

qt_add_executable(smtpUI
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

target_include_directories(smtpUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(smtpUI PRIVATE
    Qt::Widgets
    smtp_client_lib
)

if(WIN32)
    # Find Qt installation
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    get_filename_component(_qt_root_dir "${_qt_bin_dir}" DIRECTORY)
    
    # Copy platform plugin for Debug builds
    add_custom_command(TARGET smtpUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "$<TARGET_FILE_DIR:smtpUI>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_qt_root_dir}/plugins/platforms/qwindowsd$<$<CONFIG:Debug>:d>.dll"
            "$<TARGET_FILE_DIR:smtpUI>/platforms/"
        COMMENT "Copying Qt platform plugins..."
    )
    
    # Find and run windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        add_custom_command(TARGET smtpUI POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --dir "$<TARGET_FILE_DIR:smtpUI>"
                --no-compiler-runtime
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:smtpUI>"
            COMMENT "Deploying Qt dependencies..."
        )
    else()
        message(WARNING "windeployqt not found!")
    endif()
endif()

qt_finalize_executable(smtpUI)