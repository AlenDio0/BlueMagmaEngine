@echo off

set "BUILD_DIR=build_windows"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    goto :cmake
)

set /p answer="Do you want to cleanup last build? [y/N]: "

if /i "%answer%"=="y" (
    echo Cleaning last build...
    rmdir /s /q "%BUILD_DIR%"
    mkdir "%BUILD_DIR%"
) else (
    echo Cleanup last build - skipped.
)

:cmake

cd "%BUILD_DIR%"

echo Starting CMake configuration in "%BUILD_DIR%"...
cmake ..

if %errorlevel% neq 0 (
    echo Error during CMake configuration!
    pause
    exit /b 1
)

set /p answer="Do you want to build the project? [y/N]: "

if /i "%answer%"=="y" (
    echo Starting build...
    cmake --build . --config Release

    if %errorlevel% equ 0 (
        echo Build completed!
    ) else (
        echo Error during build!
        pause
        exit /b 1
    )
) else (
    echo Build project - skipped.
)

echo Building finished!
pause
exit /b 0
