cmake_minimum_required(VERSION 3.24)

project("ZNote-dev" LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# -------------------------------
# vcpkg
# -------------------------------
# 如果没有定义 CMAKE_TOOLCHAIN_FILE，就设置为项目根目录下的 vcpkg_installed
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
         "C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# 设置 vcpkg 本地安装目录
set(ENV{VCPKG_ROOT} "${CMAKE_SOURCE_DIR}/vcpkg_installed/vcpkg")
set(ENV{VCPKG_INSTALLED_DIR} "${CMAKE_SOURCE_DIR}/vcpkg_installed")


# 在 CMakeLists.txt 顶部
set(CMAKE_PREFIX_PATH "C:/C/Qt/ZNote-dev/vcpkg_installed/x64-windows/share" ${CMAKE_PREFIX_PATH})

# 添加自定义 CMake 模块路径
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/ffmpeg" ${CMAKE_MODULE_PATH})

# -------------------------------
# cmark
# -------------------------------
find_package(cmark CONFIG REQUIRED)

# -------------------------------
# FFmpeg
# -------------------------------
find_package(FFMPEG REQUIRED)





add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/qmarkdowntextedit")
add_subdirectory("ZNote-dev")