@echo off
setlocal enabledelayedexpansion

set "ROOT_DIR=%~dp0"

echo ============================================================
echo   Locale Emulator DLL - Build Script
echo ============================================================

:: 1. Check if 32-bit compiler exists in PATH
set "FOUND_COMPILER=0"

where i686-w64-mingw32-gcc >nul 2>&1 && set "FOUND_COMPILER=1"
where i686-w64-mingw32-clang >nul 2>&1 && set "FOUND_COMPILER=1"
where gcc >nul 2>&1 && set "FOUND_COMPILER=1"
where clang >nul 2>&1 && set "FOUND_COMPILER=1"

:: 2. Check if local 3rdparty toolchain is present (optional local fallback)
if "%FOUND_COMPILER%"=="0" (
    for /d %%D in ("%ROOT_DIR%3rdparty\llvm-mingw*") do (
        if exist "%%D\bin" (
            set "PATH=%%D\bin;!PATH!"
            set "FOUND_COMPILER=1"
        )
    )
)

:: 3. If still not found, show instructions
if "%FOUND_COMPILER%"=="0" (
    echo [ERROR] 32-bit MinGW toolchain not found!
    echo.
    echo Please install a 32-bit MinGW toolchain ^(e.g., llvm-mingw-i686 or MSYS2 mingw32^)
    echo and add its bin folder to your system PATH.
    echo.
    echo Recommended toolchain:
    echo   https://github.com/mstorsjo/llvm-mingw/releases ^(i686 release^)
    exit /b 1
)

:: 4. Detect CMake generator
set "CMAKE_GEN="
where ninja >nul 2>&1 && set "CMAKE_GEN=-G Ninja"
if "%CMAKE_GEN%"=="" (
    where mingw32-make >nul 2>&1 && set "CMAKE_GEN=-G \"MinGW Makefiles\""
)

if not exist "%ROOT_DIR%build" mkdir "%ROOT_DIR%build"
cd /d "%ROOT_DIR%build"

echo [1/2] Running CMake configuration...
cmake %CMAKE_GEN% -DCMAKE_BUILD_TYPE=Release "%ROOT_DIR%"
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

echo [2/2] Building project...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo.
echo ============================================================
echo [SUCCESS] Build completed successfully!
echo Deliverables generated under: %ROOT_DIR%build\bin\
echo ============================================================
exit /b 0
