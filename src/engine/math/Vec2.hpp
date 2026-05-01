#pragma once

#include <cmath>

namespace engine
{
    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;

        constexpr Vec2() = default;

        constexpr Vec2(float xValue, float yValue)
            : x(xValue)
            , y(yValue)
        {
        }
    };

    inline Vec2 operator+(Vec2 lhs, Vec2 rhs)
    {
        return Vec2 { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    inline Vec2 operator-(Vec2 lhs, Vec2 rhs)
    {
        return Vec2 { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    inline Vec2 operator*(Vec2 lhs, float scalar)
    {
        return Vec2 { lhs.x * scalar, lhs.y * scalar };
    }

    inline Vec2& operator+=(Vec2& lhs, Vec2 rhs)
    {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    inline float lengthSquared(Vec2 value)
    {
        return value.x * value.x + value.y * value.y;
    }

    inline Vec2 normalized(Vec2 value)
    {
        const float squared = lengthSquared(value);
        if (squared <= 0.0f)
        {
            return Vec2 {};
        }

        const float inverseLength = 1.0f / std::sqrt(squared);
        return Vec2 { value.x * inverseLength, value.y * inverseLength };
    }
}
