#pragma once

namespace editor
{
    class Line;
    class Rectangle;
    class Circle;
    class Ellipse;
    class Polygon;

    class IShapeVisitor
    {
    public:
        virtual ~IShapeVisitor() = default;

        virtual void visit(const Line& shape) = 0;
        virtual void visit(const Rectangle& shape) = 0;
        virtual void visit(const Circle& shape) = 0;
        virtual void visit(const Ellipse& shape) = 0;
        virtual void visit(const Polygon& shape) = 0;
    };
}
