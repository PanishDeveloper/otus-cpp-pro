#pragma once

#include "geometry.hpp"
#include <memory>
#include <cstdint>

namespace editor
{
    class IRenderer;
    class IShapeVisitor;

    using ShapeID = std::uint64_t;

    class Shape
    {
    public:
        virtual ~Shape() = default;
        virtual void render(IRenderer& renderer) const = 0;
        virtual void accept(IShapeVisitor& visitor) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Shape> clone() const = 0;
        [[nodiscard]] virtual ShapeType type() const noexcept = 0;

        [[nodiscard]] ShapeID id() const noexcept { return m_id; }
        void setID(ShapeID id) noexcept { m_id = id; }

        [[nodiscard]] bool isSelected() const noexcept { return m_selected; }
        void setSelected(bool s) noexcept { m_selected = s; }

        [[nodiscard]] const Color& color() const noexcept { return m_color; }
        void setColor(const Color& c) noexcept { m_color = c; }

    protected:
        Shape() = default;
        Shape(const Shape&) = default;
        Shape& operator=(const Shape&) = default;

    private:
        ShapeID m_id = 0;
        bool m_selected = false;
        Color m_color{};
    };

}