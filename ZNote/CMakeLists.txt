cmake_minimum_required(VERSION 3.24)

project(ZNote 
    VERSION 1.0.0
    DESCRIPTION "A modern desktop application for downloading videos from Bilibili and other platforms"
    HOMEPAGE_URL "https://github.com/1716285375/ZNote"
    LANGUAGES CXX
)

# -------------------------
# Qt6 基础
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# -------------------------
# 源文件列表
# -------------------------
set(PROJECT_SOURCES
    # 主程序入口
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    
    # 应用程序核心
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app/application.h
    
    # UI层
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mainwindow.ui
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/downloadwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/widgets/downloadwidget.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/historywidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/widgets/historywidget.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/settingswidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/widgets/settingswidget.h
    
    # 服务层
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/configservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services/configservice.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/downloadservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services/downloadservice.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/historyservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services/historyservice.h
    
    # 核心层 - 下载
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/download/videodownloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/download/videodownloader.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/download/taskqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/download/taskqueue.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/download/urlparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/download/urlparser.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/download/task.h

    # 核心层 - 接口
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/interfaces/iconfigservice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/interfaces/idownloadservice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/interfaces/ihistoryservice.h
    
    # 组件层
    ${CMAKE_CURRENT_SOURCE_DIR}/src/component/videomodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/component/videomodel.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/component/historymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/component/historymodel.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/component/checkboxdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/component/checkboxdelegate.h

    # 工具层
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/logger.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/stylemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/stylemanager.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/downloadutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/downloadutils.h

    # 资源文件
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/style/light.qss
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/style/dark.qss
)

# -------------------------
# 创建可执行文件
# -------------------------
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# -------------------------
# 包含目录
# -------------------------
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------
# 链接库
# -------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
)

# -------------------------
# 目标属性
# -------------------------
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

# -------------------------
# 后构建命令
# -------------------------
# 将 config.json.example 拷贝到可执行文件输出目录（如果不存在 config.json）
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/config.json.example"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/config.json.example"
    COMMENT "Copying config.json.example to output directory"
    )


# -------------------------
# 编译选项
# -------------------------
if(MSVC)
    # 设置C++标准库路径
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /Zc:__cplusplus
        /permissive-
        /utf-8
    )
    
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall
        -Wextra
        -Wpedantic
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    endif()
endif()

# -------------------------
# 安装规则
# -------------------------
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install resources
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/config.json.example
    DESTINATION bin
    RENAME config.json.example
)

# Install README and LICENSE
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION .
)
