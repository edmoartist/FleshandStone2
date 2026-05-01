# Repository Map

## Project

Name:

```text
isaac_engine_base
```

Engine/framework:

```text
Custom C++20 engine foundation using SDL2 backend.
```

Primary language:

```text
C++20
```

Target platform:

```text
Windows first.
```

## Main Entrypoint

```text
src/main.cpp
```

Purpose:

```text
Creates the game instance, configures the application, and starts the main loop.
```

## Engine Folders

```text
src/engine/core
```

Application lifecycle, game interface, logging.

```text
src/engine/platform
```

Window and SDL platform setup.

```text
src/engine/renderer
```

2D rendering abstraction.

```text
src/engine/input
```

Keyboard input state.

```text
src/engine/math
```

Small math types.

## Game Folders

```text
src/game
```

Current milestone game layer. Will later split into player, enemies, combat, rooms, items, UI, and states.

## Data Folders

```text
data/items
data/enemies
data/rooms
data/balance
```

Reserved for later data-driven content.

## Asset Folders

```text
assets/sprites
assets/audio
assets/fonts
assets/tilesets
```

Reserved for later runtime assets.
