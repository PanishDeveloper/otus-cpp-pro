#pragma once

#include "shape.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace editor
{
    class Document
    {
    public:
        Document() = default;
        Document(const Document&) = delete;
        Document& operator=(const Document&) = delete;

        ShapeID addShape(std::unique_ptr<Shape> shape)
        {
            if (!shape) return 0;
            const ShapeID id = m_nextID++;
            shape->setID(id);
            m_shapes.push_back(std::move(shape));
            return id;
        }

        bool removeShape(ShapeID id)
        {
            for (auto it = begin(m_shapes); it != end(m_shapes); ++it)
            {
                if ((*it)->id() == id)
                {
                    m_shapes.erase(it);
                    return true;
                }
            }
            return false;
        }

        Shape* findShape(ShapeID id) noexcept
        {
            for (auto& s : m_shapes)
                if (s->id() == id) return s.get();
            return nullptr;
        }

        [[nodiscard]] const Shape* findShape(ShapeID id) const noexcept
        {
            for (auto& s : m_shapes)
            {
                if (s->id() == id)
                    return s.get();
            }
            return nullptr;
        }

        void clear() noexcept
        {
            m_shapes.clear();
            m_nextID = 1;
        }

        [[nodiscard]] const std::vector<std::unique_ptr<Shape>>& shapes() const noexcept { return  m_shapes; }
        [[nodiscard]] std::size_t size() const noexcept { return m_shapes.size(); }
        [[nodiscard]] bool empty() const noexcept { return m_shapes.empty(); }

    private:
        std::vector<std::unique_ptr<Shape>> m_shapes;
        ShapeID m_nextID = 1;
    };
}