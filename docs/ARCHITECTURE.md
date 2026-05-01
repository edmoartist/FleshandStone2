# Architecture

## Goal

Build a small C++20 2D top-down game engine base for an Isaac-like room-based roguelite.

Milestone 0/1 proves:

- Buildable CMake project.
- SDL2 window lifecycle.
- Fixed timestep game loop.
- Input polling.
- Basic renderer abstraction.
- Engine/game code separation.
- Simple debug visibility.

## Constraints

- Language: C++20.
- Platform: Windows first.
- Build system: CMake.
- Backend: SDL2.
- No required assets for the first milestone.
- No ECS yet.
- No external JSON/content pipeline yet.
- Keep engine modules reusable and free from game-specific roguelite logic.

## Recommended Architecture

The project is split into:

- `src/engine`: reusable engine foundation.
- `src/game`: game-specific rules and prototype gameplay.
- `assets`: runtime assets.
- `data`: future data-driven definitions.
- `docs`: project documentation.
- `scripts`: automation.

## Current Systems

### Application

Owns the main loop and fixed timestep.

Files:

- `src/engine/core/Application.hpp`
- `src/engine/core/Application.cpp`

### Window

Owns SDL initialization and SDL window lifetime.

Files:

- `src/engine/platform/Window.hpp`
- `src/engine/platform/Window.cpp`

### Renderer2D

Wraps SDL renderer calls behind a small engine-facing API.

Files:

- `src/engine/renderer/Renderer2D.hpp`
- `src/engine/renderer/Renderer2D.cpp`

### Input

Tracks current and previous key state.

Files:

- `src/engine/input/Input.hpp`
- `src/engine/input/Input.cpp`
- `src/engine/input/Key.hpp`

### Game

Temporary milestone gameplay layer that moves a player rectangle inside a room rectangle.

Files:

- `src/game/Game.hpp`
- `src/game/Game.cpp`

## Runtime Flow

1. `main.cpp` constructs `game::Game`.
2. `engine::Application` creates the SDL window and renderer.
3. App enters the main loop.
4. Input events are polled.
5. Variable update runs once per rendered frame.
6. Fixed update runs at 60 Hz.
7. Renderer clears screen.
8. Game draws placeholder room/player rectangles.
9. Renderer presents frame.
10. Escape or window close exits.

## Next Systems

- Texture loading.
- Sprite rendering.
- Camera transform.
- Collision helpers.
- Projectiles.
- Enemy base class.
- Room template loading.
