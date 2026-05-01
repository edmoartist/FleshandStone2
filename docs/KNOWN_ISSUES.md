# Known Issues

## Text Rendering Not Implemented

Debug information is currently printed to the console and window title.

Reason:
Text rendering requires either a bitmap font implementation or SDL_ttf. That dependency is intentionally delayed.

## No Texture Loading Yet

The renderer only supports filled rectangles and outlines.

Reason:
Milestone 0/1 should prove lifecycle, timing, input, and rendering before adding assets.

## SDL2 Backend Leaks Into Some Engine Headers

`Input` and `Renderer2D` currently expose SDL types in engine-level headers.

Reason:
This is acceptable for the first backend milestone. If another backend is added later, introduce a stricter backend boundary.

## No Game State Stack Yet

Only one game object is used.

Reason:
The state stack should be added when menus, pause, room gameplay, and run state need separation.
