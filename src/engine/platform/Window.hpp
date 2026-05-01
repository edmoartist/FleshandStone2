#pragma once

#include <SDL.h>

#include <string>

namespace engine
{
    class Window final
    {
    public:
        Window(const std::string& title, int width, int height);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        SDL_Window* nativeHandle() const;

        int width() const;
        int height() const;

        void setTitle(const std::string& title);

    private:
        SDL_Window* _window = nullptr;
        int _width = 0;
        int _height = 0;
    };
}
