@echo off

set "BUILD_DIR=build_windows"

if exist "%BUILD_DIR%" (
    set /p answer="Do you want to cleanup last build? [y/N]: "

    if /i "%answer%"=="y" (
        echo Cleaning last build...
        rmdir /s /q "%BUILD_DIR%"
    ) else (
        echo "Cleanup last build - skipped."
    )
) else (
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo Starting CMake configuration in "%BUILD_DIR%"...
cmake ..

if %errorlevel% neq 0 (
    echo Error during CMake configuration!
    pause
    exit /b 1
)

echo Starting build...
cmake --build . --config Release

if %errorlevel% equ 0 (
    echo Build completed!
    pause
    exit /b 0
) else (
    echo Error during build!
    pause
    exit /b 1
)
