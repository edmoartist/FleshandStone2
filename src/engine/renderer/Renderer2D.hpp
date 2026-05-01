#pragma once

#include "engine/math/Rect.hpp"
#include "engine/renderer/Color.hpp"

#include <SDL.h>

namespace engine
{
    class Window;

    class Renderer2D final
    {
    public:
        explicit Renderer2D(const Window& window);
        ~Renderer2D();

        Renderer2D(const Renderer2D&) = delete;
        Renderer2D& operator=(const Renderer2D&) = delete;

        void beginFrame(Color clearColor);
        void endFrame();

        void drawFilledRect(Rect rect, Color color);
        void drawRect(Rect rect, Color color);

        SDL_Renderer* nativeHandle() const;

    private:
        SDL_Renderer* _renderer = nullptr;
    };
}
