#include "engine/core/Application.hpp"

#include "engine/core/Log.hpp"
#include "engine/input/Key.hpp"
#include "engine/renderer/Color.hpp"

#include <SDL.h>

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <utility>

namespace engine
{
    Application::Application(ApplicationConfig config, IGame& game)
        : _config(std::move(config))
        , _game(game)
        , _window(_config.title, _config.width, _config.height)
        , _renderer(_window)
    {
    }

    int Application::run()
    {
        Log::info("Application started.");
        _game.onStart();

        const double performanceFrequency = static_cast<double>(SDL_GetPerformanceFrequency());
        std::uint64_t previousCounter = SDL_GetPerformanceCounter();

        double accumulatorSeconds = 0.0;
        double debugTimerSeconds = 0.0;

        int renderedFrames = 0;
        int fixedUpdates = 0;

        while (_running)
        {
            _input.beginFrame();

            SDL_Event event {};
            while (SDL_PollEvent(&event) != 0)
            {
                _input.handleEvent(event);
            }

            if (_input.quitRequested() || _input.wasPressed(Key::Escape))
            {
                _running = false;
                break;
            }

            const std::uint64_t currentCounter = SDL_GetPerformanceCounter();
            double frameSeconds =
                static_cast<double>(currentCounter - previousCounter) / performanceFrequency;
            previousCounter = currentCounter;

            frameSeconds = std::min(frameSeconds, _config.maxFrameDeltaSeconds);

            _game.update(static_cast<float>(frameSeconds), _input);

            accumulatorSeconds += frameSeconds;

            while (accumulatorSeconds >= _config.fixedDeltaSeconds)
            {
                _game.fixedUpdate(static_cast<float>(_config.fixedDeltaSeconds), _input);
                accumulatorSeconds -= _config.fixedDeltaSeconds;
                ++fixedUpdates;
            }

            const float interpolationAlpha =
                static_cast<float>(accumulatorSeconds / _config.fixedDeltaSeconds);

            _renderer.beginFrame(Color { 13, 13, 18, 255 });
            _game.render(_renderer, interpolationAlpha);
            _renderer.endFrame();

            ++renderedFrames;
            debugTimerSeconds += frameSeconds;

            if (debugTimerSeconds >= 1.0)
            {
                std::ostringstream title;
                title << _config.title
                      << " | FPS: " << renderedFrames
                      << " | Fixed updates: " << fixedUpdates;

                _window.setTitle(title.str());
                Log::info(title.str());

                renderedFrames = 0;
                fixedUpdates = 0;
                debugTimerSeconds = 0.0;
            }
        }

        Log::info("Application stopped.");
        return 0;
    }
}
