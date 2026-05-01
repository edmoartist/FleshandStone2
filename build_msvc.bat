@echo off
setlocal

if not exist build mkdir build

cl /nologo /std:c++20 /EHsc /O2 /W4 /permissive- ^
  src\main.cpp ^
  /Fe:build\game.exe ^
  /link user32.lib gdi32.lib winmm.lib

if errorlevel 1 (
  echo Build failed.
  exit /b 1
)

echo Built build\game.exe
