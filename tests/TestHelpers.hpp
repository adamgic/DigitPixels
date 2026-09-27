#pragma once

#include <stdexcept>
#include <string>

inline void require(bool condition, std::string message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}
