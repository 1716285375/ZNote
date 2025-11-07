@echo off
REM 设置Visual Studio环境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

REM 设置Qt路径
set QTDIR=C:\Qt\6.9.1\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%

REM 清理旧的构建目录
if exist "out\build\debug" (
    echo Cleaning old build directory...
    rmdir /s /q "out\build\debug"
)

REM 配置CMake
echo Configuring CMake...
cmake --preset Qt-Debug

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM 编译项目
echo Building project...
cmake --build out/build/debug --target ZNote-dev

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build successful!
pause

