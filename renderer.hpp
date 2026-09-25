#pragma once

#include "geometry.hpp"
#include <vector>

namespace editor
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual void drawLine(const Point& a, const Point& b, const Color& color) = 0;
        virtual void drawRect(const Point& topLeft, const Point& bottomRight, const Color& color) = 0;

        virtual void drawEllipse(const Point& center, double rx, double ry, const Color& color) = 0;
        virtual void drawPolygon(const std::vector<Point>& points, const Color& color) = 0;
    };
}