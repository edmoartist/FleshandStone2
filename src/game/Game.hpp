#pragma once

#include "engine/core/IGame.hpp"
#include "engine/math/Rect.hpp"
#include "engine/math/Vec2.hpp"

namespace game
{
    class Game final : public engine::IGame
    {
    public:
        void onStart() override;
        void update(float deltaSeconds, const engine::Input& input) override;
        void fixedUpdate(float fixedDeltaSeconds, const engine::Input& input) override;
        void render(engine::Renderer2D& renderer, float interpolationAlpha) override;

    private:
        engine::Vec2 _playerPosition { 640.0f, 360.0f };
        engine::Rect _roomBounds { 80.0f, 60.0f, 1120.0f, 600.0f };

        float _playerSpeed = 260.0f;
        float _playerSize = 32.0f;

        bool _debugDrawEnabled = true;
    };
}
