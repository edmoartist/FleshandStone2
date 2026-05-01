@echo off
setlocal

REM Set RAYLIB_PATH if raylib is not in a default location.
REM Example:
REM   set RAYLIB_PATH=C:\raylib\raylib

where g++ >nul 2>nul
if errorlevel 1 (
  echo g++ was not found in PATH.
  echo Install MinGW-w64 or use build_msvc.bat from a Visual Studio developer prompt.
  exit /b 1
)

if not exist build mkdir build

set "RAYLIB_INCLUDE="
set "RAYLIB_LIB="

if defined RAYLIB_PATH (
  set "RAYLIB_INCLUDE=-I%RAYLIB_PATH%\src -I%RAYLIB_PATH%\include"
  set "RAYLIB_LIB=-L%RAYLIB_PATH%\src -L%RAYLIB_PATH%\lib"
)

g++ -std=c++20 -O2 -Wall -Wextra src\main.cpp -o build\game.exe %RAYLIB_INCLUDE% %RAYLIB_LIB% -lraylib -lopengl32 -lgdi32 -lwinmm

if errorlevel 1 (
  echo.
  echo Build failed.
  echo If raylib was not found, set RAYLIB_PATH first.
  echo Example:
  echo   set RAYLIB_PATH=C:\raylib\raylib
  exit /b 1
)

echo Built build\game.exe
