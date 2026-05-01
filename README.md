# FleshAndStoneCpp

Dependency-free C++20 Win32 game starter.

This project uses raw Win32 plus a software backbuffer. It is intentionally boring and portable across normal Windows C++ toolchains.

## Build with MSVC

Open **x64 Native Tools Command Prompt for VS**, then run:

```bat
build_msvc.bat
build\game.exe
```

## Build with MinGW

```bat
build_mingw.bat
build\game.exe
```

## Controls

- WASD / Arrow keys: move
- Mouse: aim
- Left click / Space: shoot
- R: restart room
- Touch teal portal after clearing enemies: next room

## Structure

```text
src/main.cpp          Native Win32 platform + game loop + software renderer
build_msvc.bat        MSVC build
build_mingw.bat       MinGW build
tests/smoke_test.py   Checks required project files/content
```

## Why one C++ file?

Because first we prove the native window, loop, input, rendering, and gameplay work. Then split it.

Recommended next split:

```text
src/platform_win32.cpp
src/game.cpp
src/game.hpp
src/renderer_software.cpp
src/renderer_software.hpp
src/math.hpp
```
