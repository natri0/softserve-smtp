cmake_minimum_required(VERSION 3.28)
project(smtp_client)

set(CMAKE_AUTOMOC ON)

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
        src/ComposeDialog.cpp
        include/ComposeDialog.h
)

target_include_directories(smtpUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(smtpUI PRIVATE
        Qt6::Core
   	Qt6::Widgets
    	Qt6::Network
	smtp_client_lib
)

# Шляхи до Qt DLL в залежності від конфігурації
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Для Debug спробуємо debug/bin, якщо немає - візьмемо Release
    if(EXISTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin/Qt6Cored.dll")
        set(QT_DLL_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin")
        set(QT_DLL_SUFFIX "d")
    else()
        message(WARNING "Debug Qt DLLs not found, using Release DLLs for Debug build")
        set(QT_DLL_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
        set(QT_DLL_SUFFIX "")
    endif()
    set(QT_PLUGINS_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/Qt6/plugins")
else()
    set(QT_DLL_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
    set(QT_DLL_SUFFIX "")
    set(QT_PLUGINS_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/plugins")
endif()

# Копіювання Qt DLLs та SSL сертифікатів
add_custom_command(TARGET smtpUI POST_BUILD
    COMMENT "Copying Qt runtime files and SSL certs..."

    # Qt платформи
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:smtpUI>/platforms"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_DLL_DIR}/Qt6Core${QT_DLL_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:smtpUI>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_DLL_DIR}/Qt6Gui${QT_DLL_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:smtpUI>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_DLL_DIR}/Qt6Widgets${QT_DLL_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:smtpUI>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_DLL_DIR}/Qt6Network${QT_DLL_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:smtpUI>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_PLUGINS_DIR}/platforms/qwindows${QT_DLL_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:smtpUI>/platforms"

    # SSL сертифікати для клієнта
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:smtpUI>/networking/SSL/certs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/networking/SSL/certs"
        "$<TARGET_FILE_DIR:smtpUI>/networking/SSL/certs"
)


include(GNUInstallDirs)
install(TARGETS smtpUI
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
