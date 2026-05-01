#pragma once

#include "engine/input/Key.hpp"

#include <SDL.h>

#include <array>
#include <cstddef>

namespace engine
{
    class Input final
    {
    public:
        void beginFrame();
        void handleEvent(const SDL_Event& event);

        bool isDown(Key key) const;
        bool wasPressed(Key key) const;
        bool wasReleased(Key key) const;

        bool quitRequested() const;

    private:
        static constexpr std::size_t KeyCount = static_cast<std::size_t>(Key::Count);

        std::array<bool, KeyCount> _currentKeys {};
        std::array<bool, KeyCount> _previousKeys {};
        bool _quitRequested = false;
    };
}
