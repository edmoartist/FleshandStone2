# Isaac Engine Base — Milestone 0/1

Small C++20 + CMake + SDL2 foundation for a 2D top-down Isaac-like engine.

## What this milestone includes

- CMake project.
- SDL2 window lifecycle.
- Fixed timestep game loop.
- Input snapshot.
- Renderer clear/present.
- Placeholder room/player rectangle rendering.
- Debug FPS/fixed update count in the window title and console.
- Clean `src/engine` and `src/game` split.

## Install SDL2 with vcpkg

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
C:\dev\vcpkg\vcpkg.exe install sdl2:x64-windows
```

## Build

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Debug
```

## Run

```powershell
.\build\Debug\isaac_engine_base.exe
```

## Controls

- `WASD` / arrow keys: move player block.
- `F1`: toggle debug outline.
- `Escape`: quit.

## Expected result

A 1280x720 window opens. A room rectangle and player square render. The player square moves inside the room bounds. The title updates once per second with FPS and fixed update count.
