#include "engine/core/Log.hpp"

#include <iostream>

namespace engine
{
    void Log::info(std::string_view message)
    {
        std::cout << "[Info] " << message << '\n';
    }

    void Log::warning(std::string_view message)
    {
        std::cout << "[Warning] " << message << '\n';
    }

    void Log::error(std::string_view message)
    {
        std::cerr << "[Error] " << message << '\n';
    }
}
