cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/SMTPClientWindow.cpp
        src/SMTPController.cpp
        src/SMTPWorker.cpp
        include/SMTPClientWindow.h
        include/SMTPController.h
        include/SMTPWorker.h
)

qt_add_resources(PROJECT_RESOURCES "resources.qrc")

qt_add_executable(smtpUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
        include/Structures.h
)

target_include_directories(smtpUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(smtpUI PRIVATE
        Qt::Widgets
        smtp_client_lib
)

if(WIN32)
    add_custom_command(TARGET smtpUI POST_BUILD
            COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe" "$<TARGET_FILE:smtpUI>"
            COMMENT "Deploying Qt runtime..."
    )
endif()

include(GNUInstallDirs)
install(TARGETS smtpUI
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
