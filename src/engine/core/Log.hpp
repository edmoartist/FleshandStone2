#pragma once

#include <string_view>

namespace engine
{
    class Log final
    {
    public:
        static void info(std::string_view message);
        static void warning(std::string_view message);
        static void error(std::string_view message);
    };
}
