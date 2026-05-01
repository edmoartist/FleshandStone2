@echo off
setlocal

REM Set RAYLIB_PATH if raylib is not in a default location.
REM Example:
REM   set RAYLIB_PATH=C:\raylib\raylib

where cl >nul 2>nul
if errorlevel 1 (
  echo MSVC compiler not found in PATH.
  echo Open "x64 Native Tools Command Prompt for VS" or run this from a configured Developer PowerShell.
  exit /b 1
)

if not exist build mkdir build

set "RAYLIB_INCLUDE="
set "RAYLIB_LIB="

if defined RAYLIB_PATH (
  set "RAYLIB_INCLUDE=/I%RAYLIB_PATH%\src /I%RAYLIB_PATH%\include"
  set "RAYLIB_LIB=/LIBPATH:%RAYLIB_PATH%\src /LIBPATH:%RAYLIB_PATH%\lib"
)

cl /nologo /std:c++20 /EHsc /O2 /W4 /permissive- ^
  %RAYLIB_INCLUDE% ^
  src\main.cpp ^
  /Fe:build\game.exe ^
  /link %RAYLIB_LIB% raylib.lib winmm.lib gdi32.lib user32.lib shell32.lib opengl32.lib

if errorlevel 1 (
  echo.
  echo Build failed.
  echo If raylib was not found, set RAYLIB_PATH first.
  echo Example:
  echo   set RAYLIB_PATH=C:\raylib\raylib
  exit /b 1
)

echo Built build\game.exe
