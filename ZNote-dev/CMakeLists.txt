cmake_minimum_required(VERSION 3.16)
project(ZNote-dev LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Sql

)
qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/videodownloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/videodownloader.h

    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/style/app.qss

    ${CMAKE_CURRENT_SOURCE_DIR}/include/task.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/taskqueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskqueue.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/configmanager.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/downloadmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/downloadmanager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/urlparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/urlparser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/interface/ihistorybackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/implement/jsonhistorybackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/implement/jsonhistorybackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/implement/sqlitehisotrybackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/implement/sqlitehistorybackend.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/historymanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/historymanager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/misc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 添加 include 目录到头文件路径
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Sql

)
