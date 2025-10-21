@echo off
REM Build script for FQDN Blocker CLI

echo ========================================
echo   FQDN Blocker CLI - Build Script
echo ========================================
echo.

REM Check for Administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Warning: Not running as Administrator
    echo The built executable will require Administrator privileges to run.
    echo.
)

REM Check if json.hpp exists
if not exist "lib\json.hpp" (
    echo Error: json.hpp not found in lib directory
    echo.
    echo Please run: powershell -ExecutionPolicy Bypass -File download_dependencies.ps1
    echo Or manually download from: https://github.com/nlohmann/json/releases/latest/download/json.hpp
    echo.
    pause
    exit /b 1
)

echo [1/3] Checking for existing build directory...
if exist "build" (
    echo Build directory exists
    choice /C YN /M "Do you want to clean and rebuild"
    if errorlevel 2 goto skip_clean
    if errorlevel 1 (
        echo Cleaning build directory...
        rd /s /q build
        mkdir build
    )
) else (
    echo Creating build directory...
    mkdir build
)

:skip_clean

echo.
echo [2/3] Configuring project with CMake...
cd build
cmake .. -G "Visual Studio 16 2019" 2>nul
if %errorLevel% neq 0 (
    cmake .. -G "Visual Studio 17 2022" 2>nul
    if %errorLevel% neq 0 (
        cmake ..
        if %errorLevel% neq 0 (
            echo.
            echo Error: CMake configuration failed
            echo Make sure CMake and Visual Studio are installed
            cd ..
            pause
            exit /b 1
        )
    )
)

echo.
echo [3/3] Building project (Release configuration)...
cmake --build . --config Release

if %errorLevel% equ 0 (
    echo.
    echo ========================================
    echo   Build completed successfully!
    echo ========================================
    echo.
    echo Executable location: build\Release\FqdnBlockerCli.exe
    echo.
    echo To run the application:
    echo   1. Right-click FqdnBlockerCli.exe
    echo   2. Select "Run as administrator"
    echo.
    echo Or from elevated PowerShell:
    echo   .\FqdnBlockerCli.exe help
    echo.
) else (
    echo.
    echo ========================================
    echo   Build failed!
    echo ========================================
    echo.
    echo Please check the error messages above.
    echo.
)

cd ..
pause
