#pragma once

#include <cstdint>

namespace editor
{
    struct Point { double x = 0.0; double y = 0.0; };

    struct Color
    {
        std::uint8_t r = 0;
        std::uint8_t g = 0;
        std::uint8_t b = 0;
        std::uint8_t a = 255;
    };

    enum class ShapeType { Line, Rectangle, Circle, Ellipse, Polygon };
}