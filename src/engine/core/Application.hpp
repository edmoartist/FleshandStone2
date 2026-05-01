#pragma once

#include "engine/core/IGame.hpp"
#include "engine/input/Input.hpp"
#include "engine/platform/Window.hpp"
#include "engine/renderer/Renderer2D.hpp"

#include <string>

namespace engine
{
    struct ApplicationConfig
    {
        std::string title = "Isaac Engine Base";
        int width = 1280;
        int height = 720;

        double fixedDeltaSeconds = 1.0 / 60.0;
        double maxFrameDeltaSeconds = 0.25;
    };

    class Application final
    {
    public:
        Application(ApplicationConfig config, IGame& game);
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        int run();

    private:
        ApplicationConfig _config;
        IGame& _game;
        Window _window;
        Renderer2D _renderer;
        Input _input;
        bool _running = true;
    };
}
