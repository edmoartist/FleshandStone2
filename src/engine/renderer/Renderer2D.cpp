#include "engine/renderer/Renderer2D.hpp"

#include "engine/core/Log.hpp"
#include "engine/platform/Window.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
    SDL_Rect toSdlRect(engine::Rect rect)
    {
        return SDL_Rect {
            static_cast<int>(std::round(rect.x)),
            static_cast<int>(std::round(rect.y)),
            static_cast<int>(std::round(rect.w)),
            static_cast<int>(std::round(rect.h))
        };
    }

    void setDrawColor(SDL_Renderer* renderer, engine::Color color)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }
}

namespace engine
{
    Renderer2D::Renderer2D(const Window& window)
    {
        _renderer = SDL_CreateRenderer(
            window.nativeHandle(),
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (_renderer == nullptr)
        {
            Log::warning(
                std::string("Accelerated vsync renderer failed, trying fallback renderer: ") +
                SDL_GetError()
            );

            _renderer = SDL_CreateRenderer(window.nativeHandle(), -1, 0);
        }

        if (_renderer == nullptr)
        {
            throw std::runtime_error(
                std::string("SDL_CreateRenderer failed: ") + SDL_GetError()
            );
        }

        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    }

    Renderer2D::~Renderer2D()
    {
        if (_renderer != nullptr)
        {
            SDL_DestroyRenderer(_renderer);
            _renderer = nullptr;
        }
    }

    void Renderer2D::beginFrame(Color clearColor)
    {
        setDrawColor(_renderer, clearColor);
        SDL_RenderClear(_renderer);
    }

    void Renderer2D::endFrame()
    {
        SDL_RenderPresent(_renderer);
    }

    void Renderer2D::drawFilledRect(Rect rect, Color color)
    {
        const SDL_Rect sdlRect = toSdlRect(rect);
        setDrawColor(_renderer, color);
        SDL_RenderFillRect(_renderer, &sdlRect);
    }

    void Renderer2D::drawRect(Rect rect, Color color)
    {
        const SDL_Rect sdlRect = toSdlRect(rect);
        setDrawColor(_renderer, color);
        SDL_RenderDrawRect(_renderer, &sdlRect);
    }

    SDL_Renderer* Renderer2D::nativeHandle() const
    {
        return _renderer;
    }
}
