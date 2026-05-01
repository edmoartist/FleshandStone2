#include "engine/core/Application.hpp"
#include "engine/core/Log.hpp"
#include "game/Game.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    try
    {
        game::Game game;

        engine::ApplicationConfig config;
        config.title = "Isaac Engine Base";
        config.width = 1280;
        config.height = 720;

        engine::Application app(config, game);
        return app.run();
    }
    catch (const std::exception& exception)
    {
        engine::Log::error(exception.what());
        return 1;
    }
    catch (...)
    {
        engine::Log::error("Unknown fatal error.");
        return 1;
    }
}
