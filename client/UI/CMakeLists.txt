cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

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
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
)

target_include_directories(smtpUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(smtpUI PRIVATE
        Qt6::Core
   	Qt6::Widgets
    	Qt6::Network
	smtp_client_lib
)

include(GNUInstallDirs)
install(TARGETS smtpUI
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)