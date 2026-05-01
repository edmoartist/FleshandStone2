#include "game/Game.hpp"

#include "engine/core/Log.hpp"
#include "engine/input/Input.hpp"
#include "engine/input/Key.hpp"
#include "engine/renderer/Color.hpp"
#include "engine/renderer/Renderer2D.hpp"

#include <algorithm>

namespace game
{
    void Game::onStart()
    {
        engine::Log::info("Game started. Move with WASD/arrows. F1 toggles debug draw. Escape quits.");
    }

    void Game::update(float deltaSeconds, const engine::Input& input)
    {
        (void)deltaSeconds;

        if (input.wasPressed(engine::Key::F1))
        {
            _debugDrawEnabled = !_debugDrawEnabled;
            engine::Log::info(_debugDrawEnabled ? "Debug draw enabled." : "Debug draw disabled.");
        }
    }

    void Game::fixedUpdate(float fixedDeltaSeconds, const engine::Input& input)
    {
        engine::Vec2 movement {};

        if (input.isDown(engine::Key::W) || input.isDown(engine::Key::Up))
        {
            movement.y -= 1.0f;
        }

        if (input.isDown(engine::Key::S) || input.isDown(engine::Key::Down))
        {
            movement.y += 1.0f;
        }

        if (input.isDown(engine::Key::A) || input.isDown(engine::Key::Left))
        {
            movement.x -= 1.0f;
        }

        if (input.isDown(engine::Key::D) || input.isDown(engine::Key::Right))
        {
            movement.x += 1.0f;
        }

        if (engine::lengthSquared(movement) > 0.0f)
        {
            movement = engine::normalized(movement);
        }

        _playerPosition += movement * _playerSpeed * fixedDeltaSeconds;

        const float halfSize = _playerSize * 0.5f;

        _playerPosition.x = std::clamp(
            _playerPosition.x,
            _roomBounds.x + halfSize,
            _roomBounds.x + _roomBounds.w - halfSize
        );

        _playerPosition.y = std::clamp(
            _playerPosition.y,
            _roomBounds.y + halfSize,
            _roomBounds.y + _roomBounds.h - halfSize
        );
    }

    void Game::render(engine::Renderer2D& renderer, float interpolationAlpha)
    {
        (void)interpolationAlpha;

        renderer.drawFilledRect(_roomBounds, engine::Color { 24, 24, 34, 255 });
        renderer.drawRect(_roomBounds, engine::Color { 90, 90, 110, 255 });

        const float halfSize = _playerSize * 0.5f;

        const engine::Rect playerRect {
            _playerPosition.x - halfSize,
            _playerPosition.y - halfSize,
            _playerSize,
            _playerSize
        };

        renderer.drawFilledRect(playerRect, engine::Color { 190, 70, 95, 255 });

        if (_debugDrawEnabled)
        {
            renderer.drawRect(playerRect, engine::Color { 255, 255, 255, 255 });
        }
    }
}
