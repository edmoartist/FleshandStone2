# Implementation Plan

## Milestone 0: Buildable Empty App

Status: Done.

- CMake project.
- SDL2 dependency.
- Application starts and exits cleanly.

## Milestone 1: Window, Loop, Input, Timing

Status: Done.

- Window opens.
- Fixed timestep runs at 60 Hz.
- Input supports WASD/arrows, F1, Escape.
- Renderer clears and presents.
- Debug FPS/update count visible in title and console.

## Milestone 2: Sprite Rendering and Resource Loading

Next.

Tasks:

- Add texture wrapper.
- Add resource cache.
- Load BMP first using core SDL only, or add SDL_image explicitly.
- Draw textured sprite.
- Fail clearly when assets are missing.

## Milestone 3: Player Movement and Camera

Tasks:

- Move player logic into `src/game/player`.
- Add camera transform.
- Add world-to-screen conversion.
- Add simple room coordinate system.

## Milestone 4: Collision and Projectiles

Tasks:

- Add AABB collision helpers.
- Add projectile object.
- Add projectile debug draw.
- Add projectile lifetime cleanup.

## Milestone 5: Enemy Spawning and Damage

Tasks:

- Add enemy base class.
- Add simple chase enemy.
- Add health/damage structs.
- Add enemy death cleanup.

## Milestone 6: Room Templates and Transitions

Tasks:

- Add room model.
- Add door model.
- Add authored room templates.
- Add room transition logic.
