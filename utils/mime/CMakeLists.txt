cmake_minimum_required(VERSION 3.30)
project(MimeLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Статична бібліотека
add_library(mime STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/EmailBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/EmailMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/MimeFormatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/MimeParser.cpp
)

# Заголовки для інших проєктів
target_include_directories(mime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(mime PUBLIC Boost::uuid)

# Кросплатформові визначення
if(WIN32)
    target_compile_definitions(mime PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
