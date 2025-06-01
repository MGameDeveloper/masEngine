@echo off
:: Build script using CMake and Ninja

set ENGINE_NAME=masEngine
set BUILD_DIR=SrcBuild
set GENERATOR="Ninja"
set CMAKE_BUILD_TYPE=Debug

echo Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: Configure the project with CMake
echo Configure project with CMake...
cmake -B "%BUILD_DIR%" -G "%GENERATOR%" -DCMAKE_BUILD_TYPE="%CMAKE_BUILD_TYPE%"

if %errorlevel% neq 0 (
    echo CMake configuration failed
    exit /b %errorlevel%
)


:: Build the project with Ninja
echo Building project with Ninja...
cmake --build "%BUILD_DIR%" --config %CMAKE_BUILD_TYPE%

if %errorlevel% neq 0 (
    echo Ninja build failed
    exit /b %errorlevel%
)

echo Build completed successfully

start "" "%~dp0%BUILD_DIR%\%ENGINE_NAME%"