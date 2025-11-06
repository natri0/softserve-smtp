cmake_minimum_required(VERSION 3.30)
project(smtp_client)

set(PROJECT_SOURCES
    src/main.cpp
    src/SMTPClientWindow.cpp
    src/SMTPController.cpp
    src/SMTPWorker.cpp
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

include(GNUInstallDirs)
install(TARGETS smtpUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(smtpUI)