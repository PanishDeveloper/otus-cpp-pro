#pragma once

#include "shape.hpp"
#include "shape_visitor.hpp"
#include "renderer.hpp"
#include <vector>

namespace editor
{
    class Line final : public Shape
    {
    public:
        void render(IRenderer& renderer) const override
        {
            renderer.drawLine(m_a, m_b, color());
        }
        void accept(IShapeVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Line>(*this); }
        [[nodiscard]] ShapeType type() const noexcept override { return ShapeType::Line; }

    private:
        Point m_a{};
        Point m_b{};
    };

    class Rectangle final : public Shape
    {
    public:
        void render(IRenderer& renderer) const override
        {
            renderer.drawRect(m_topLeft, m_bottomRight, color());
        }
        void accept(IShapeVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Rectangle>(*this); }
        [[nodiscard]] ShapeType type() const noexcept override { return ShapeType::Rectangle; }

    private:
        Point m_topLeft{};
        Point m_bottomRight{};
    };

    class Circle final : public Shape
    {
    public:
        void render(IRenderer& renderer) const override
        {
            renderer.drawEllipse(m_center, m_radius, m_radius, color());
        }
        void accept(IShapeVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Circle>(*this); }
        [[nodiscard]] ShapeType type() const noexcept override { return ShapeType::Circle; }

    private:
        Point m_center{};
        double m_radius = 0.0;
    };

    class Ellipse final : public Shape
    {
    public:
        void render(IRenderer& renderer) const override
        {
            renderer.drawEllipse(m_center, m_rx, m_ry, color());
        }
        void accept(IShapeVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Ellipse>(*this); }
        [[nodiscard]] ShapeType type() const noexcept override { return ShapeType::Ellipse; }

    private:
        Point m_center{};
        double m_rx = 0.0;
        double m_ry = 0.0;
    };

    class Polygon final : public Shape
    {
    public:
        void render(IRenderer& renderer) const override
        {
            renderer.drawPolygon(m_points, color());
        }
        void accept(IShapeVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Polygon>(*this); }
        [[nodiscard]] ShapeType type() const noexcept override { return ShapeType::Polygon; }

    private:
        std::vector<Point> m_points;
    };


    [[nodiscard]] inline std::unique_ptr<Shape> makeShape(ShapeType type)
    {
        switch (type)
        {
        case ShapeType::Line:      return std::make_unique<Line>();
        case ShapeType::Rectangle: return std::make_unique<Rectangle>();
        case ShapeType::Circle:    return std::make_unique<Circle>();
        case ShapeType::Ellipse:   return std::make_unique<Ellipse>();
        case ShapeType::Polygon:   return std::make_unique<Polygon>();
        }
        return nullptr;
    }
}
