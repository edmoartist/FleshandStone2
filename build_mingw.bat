@echo off
setlocal

if not exist build mkdir build

g++ -std=c++20 -O2 -Wall -Wextra -pedantic src\main.cpp -o build\game.exe -lgdi32 -luser32 -lwinmm

if errorlevel 1 (
  echo Build failed.
  exit /b 1
)

echo Built build\game.exe
