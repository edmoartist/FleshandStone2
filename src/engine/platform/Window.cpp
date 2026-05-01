#include "engine/platform/Window.hpp"

#include <stdexcept>
#include <string>

namespace engine
{
    Window::Window(const std::string& title, int width, int height)
        : _width(width)
        , _height(height)
    {
        SDL_SetMainReady();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
        {
            throw std::runtime_error(
                std::string("SDL_Init failed: ") + SDL_GetError()
            );
        }

        _window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            _width,
            _height,
            SDL_WINDOW_SHOWN
        );

        if (_window == nullptr)
        {
            const std::string error = SDL_GetError();
            SDL_Quit();
            throw std::runtime_error("SDL_CreateWindow failed: " + error);
        }
    }

    Window::~Window()
    {
        if (_window != nullptr)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }

        SDL_Quit();
    }

    SDL_Window* Window::nativeHandle() const
    {
        return _window;
    }

    int Window::width() const
    {
        return _width;
    }

    int Window::height() const
    {
        return _height;
    }

    void Window::setTitle(const std::string& title)
    {
        SDL_SetWindowTitle(_window, title.c_str());
    }
}
