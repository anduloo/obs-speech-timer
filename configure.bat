@echo off
setlocal

:: Set Qt path
set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64

:: Set OBS Studio SDK paths
set OBS_STUDIO_DIR=D:\Obs-studio\installed
set LIBOBS_INCLUDE_DIR=%OBS_STUDIO_DIR%\include
set LIBOBS_LIB_DIR=%OBS_STUDIO_DIR%\bin\64bit
set OBS_FRONTEND_API=%LIBOBS_LIB_DIR%\obs-frontend-api.dll

:: Configure with CMake
cmake -S . -B build ^
    -G "Visual Studio 17 2022" -A x64 ^
    -DLIBOBS_INCLUDE_DIR="%LIBOBS_INCLUDE_DIR%" ^
    -DLIBOBS_LIB="%LIBOBS_LIB_DIR%\obs.lib" ^
    -DOBS_FRONTEND_API="%OBS_FRONTEND_API%" ^
    -DCMAKE_PREFIX_PATH="%OBS_STUDIO_DIR%"

if errorlevel 1 (
    echo CMake configuration failed
    pause
    exit /b 1
)

echo Configuration completed successfully
pause 