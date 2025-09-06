cmake_minimum_required(VERSION 3.24)


# -------------------------
# Debug: 打印 CMake 查找策略
# -------------------------
#set(CMAKE_FIND_DEBUG_MODE TRUE)  # 会输出每一个查找尝试的目录
# Qt6 基础
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    WebEngineWidgets
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui

    ${CMAKE_CURRENT_SOURCE_DIR}/src/videodownloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/videodownloader.h

    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/style/app.qss

    ${CMAKE_CURRENT_SOURCE_DIR}/include/task.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/taskqueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskqueue.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/configmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configmanager.cpp

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

    ${CMAKE_CURRENT_SOURCE_DIR}/include/component/switchbutton.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/filebrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/filebrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/fileresolve.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/fileresolve.cpp
)



qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 添加 include 目录到头文件路径
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/vendor/qmarkdowntextedit
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)


# Qt 模块
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::WebEngineWidgets
)

# 将 config.json 拷贝到可执行文件输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/config.json"        # 源文件路径
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)              # 输出目录（可执行文件目录）


file(GLOB DLLS "C:/C/Qt/ZNote-dev/vcpkg_installed/x64-windows/bin/*.dll")
foreach(dll ${DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

target_link_libraries(${PROJECT_NAME} PRIVATE  qmarkdowntextedit)

# ------------------------
# cmark
# ------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE cmark::cmark)

# ------------------------
# FFmpeg
# ------------------------
#target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
#target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

# ------------------------
# 编译选项（可选）
# ------------------------
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    endif()
endif()
