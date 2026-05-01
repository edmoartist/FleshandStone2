#pragma once

namespace engine
{
    class Input;
    class Renderer2D;

    class IGame
    {
    public:
        virtual ~IGame() = default;

        virtual void onStart() {}
        virtual void update(float deltaSeconds, const Input& input) = 0;
        virtual void fixedUpdate(float fixedDeltaSeconds, const Input& input) = 0;
        virtual void render(Renderer2D& renderer, float interpolationAlpha) = 0;
    };
}
