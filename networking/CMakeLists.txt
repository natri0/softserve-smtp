set(BOOST_ROOT "C:/Users/alkir/boost_1_89_0/boost_1_89_0")

add_library(networking
        Session.cpp
        Session.h
)

target_include_directories(networking
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${BOOST_ROOT}
)

link_directories("${BOOST_ROOT}/stage/lib")

target_link_libraries(networking
        PUBLIC
        ws2_32
        mswsock
)