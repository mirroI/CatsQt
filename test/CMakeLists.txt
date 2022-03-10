set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(SOURCES
        main.cpp)

add_executable(${PROJEC_NAME}Test ${SOURCES})

target_link_libraries(${PROJEC_NAME}Test PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Cifrazia::${PROJECT_NAME}
        )
