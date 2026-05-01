# FleshandStone2

Native C++ raylib prototype for Flesh and Stone 2.

## Requirements

- C++20 compiler
- raylib installed locally
- Windows build scripts are included

## Build with MSVC

Open **x64 Native Tools Command Prompt for VS**, then:

```bat
build_msvc.bat
build\game.exe
```

If raylib is not in a compiler-default include/lib path, set `RAYLIB_PATH` first:

```bat
set RAYLIB_PATH=C:\raylib\raylib
build_msvc.bat
```

The script checks:

```text
%RAYLIB_PATH%\src
%RAYLIB_PATH%\include
%RAYLIB_PATH%\lib
```

## Build with MinGW

```bat
set RAYLIB_PATH=C:\raylib\raylib
build_mingw.bat
build\game.exe
```

## Controls

- WASD / Arrow keys: move
- Mouse: aim
- Left click / Space: shoot
- R: restart
- Clear all enemies, then enter the teal portal to advance rooms

## Current structure

```text
src/main.cpp       raylib prototype
build_msvc.bat    MSVC build script
build_mingw.bat   MinGW build script
tests/            repository smoke checks
```

## Next engineering step

Once the raylib build is confirmed on-machine, split `src/main.cpp` into:

```text
src/main.cpp
src/game/game.hpp
src/game/game.cpp
src/game/combat.cpp
src/game/level.cpp
src/render/render_raylib.cpp
src/core/math.hpp
```

No more raw Win32 boilerplate. We are using raylib for the platform/rendering layer.
