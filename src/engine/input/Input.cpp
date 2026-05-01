#include "engine/input/Input.hpp"

#include <optional>

namespace
{
    std::size_t keyIndex(engine::Key key)
    {
        return static_cast<std::size_t>(key);
    }

    std::optional<engine::Key> keyFromScancode(SDL_Scancode scancode)
    {
        switch (scancode)
        {
            case SDL_SCANCODE_W: return engine::Key::W;
            case SDL_SCANCODE_A: return engine::Key::A;
            case SDL_SCANCODE_S: return engine::Key::S;
            case SDL_SCANCODE_D: return engine::Key::D;

            case SDL_SCANCODE_UP: return engine::Key::Up;
            case SDL_SCANCODE_DOWN: return engine::Key::Down;
            case SDL_SCANCODE_LEFT: return engine::Key::Left;
            case SDL_SCANCODE_RIGHT: return engine::Key::Right;

            case SDL_SCANCODE_SPACE: return engine::Key::Space;
            case SDL_SCANCODE_ESCAPE: return engine::Key::Escape;
            case SDL_SCANCODE_F1: return engine::Key::F1;

            default: return std::nullopt;
        }
    }
}

namespace engine
{
    void Input::beginFrame()
    {
        _previousKeys = _currentKeys;
        _quitRequested = false;
    }

    void Input::handleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_QUIT)
        {
            _quitRequested = true;
            return;
        }

        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            _quitRequested = true;
            return;
        }

        if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        {
            return;
        }

        const std::optional<Key> key = keyFromScancode(event.key.keysym.scancode);
        if (!key.has_value())
        {
            return;
        }

        _currentKeys[keyIndex(*key)] = event.type == SDL_KEYDOWN;
    }

    bool Input::isDown(Key key) const
    {
        return _currentKeys[keyIndex(key)];
    }

    bool Input::wasPressed(Key key) const
    {
        const std::size_t index = keyIndex(key);
        return _currentKeys[index] && !_previousKeys[index];
    }

    bool Input::wasReleased(Key key) const
    {
        const std::size_t index = keyIndex(key);
        return !_currentKeys[index] && _previousKeys[index];
    }

    bool Input::quitRequested() const
    {
        return _quitRequested;
    }
}
