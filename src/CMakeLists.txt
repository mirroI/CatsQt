set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

set(CLIENT
        client/client.cpp
        client/client.h
        client/connect.cpp
        client/connect.h
        client/enums.h
        client/headers.cpp
        client/headers.h
        client/models.cpp
        client/models.h
        client/async_tcp_socket.cpp
        client/async_tcp_socket.h
        )

set(SOURCE
        cats_qt_global.h
        byte_buf.cpp
        byte_buf.h
        ${CLIENT})

add_library(${PROJECT_NAME} STATIC ${SOURCE})
add_library(Cifrazia::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        target_compile_options(${PROJECT_NAME} PRIVATE /guard:cf)
        target_link_options(${PROJECT_NAME} PRIVATE /GUARD:CF)
    endif ()
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt${QT_VERSION_MAJOR}::Network
        ${CMAKE_SOURCE_DIR}/libs/libReactorCore.a
        )

include_directories(${CMAKE_SOURCE_DIR}/includes/reactor_core)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CATS_LIBRARY
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
        )
