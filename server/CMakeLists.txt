cmake_minimum_required(VERSION 3.30)
project(smtp_server)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../3rdparty/boost_1_89_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

# Створюємо executable **спочатку**
add_executable(smtp_server
    main.cpp
    ../networking/Session.cpp
    ../networking/SSL/CryptoManager.cpp
    ../networking/SSL/SSLContextFactory.cpp
    ../networking/SSL/KeyExchanger.cpp
    Server.cpp
    ThreadPool/src/ThreadPool.cpp
    ServerConsoleUI.cpp
)

# Тепер додаємо include directories
target_include_directories(smtp_server PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}                       # корінь проекту
    ${CMAKE_SOURCE_DIR}/ThreadPool/include    # ThreadPool headers
    ${CMAKE_SOURCE_DIR}/ThreadPool/includeForTest
    ${CMAKE_SOURCE_DIR}/.. 
    ${CMAKE_SOURCE_DIR}/../networking
    ${CMAKE_SOURCE_DIR}/../networking/SSL        
)

# Лінкуємо бібліотеки
if(WIN32)
    target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        ws2_32
        mswsock
        OpenSSL::SSL
        OpenSSL::Crypto
<<<<<<< HEAD
    )
else() # Linux / macOS
    target_link_libraries(smtp_server PRIVATE
        ${Boost_LIBRARIES}
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()
=======
        config_parser
)
>>>>>>> origin/server
