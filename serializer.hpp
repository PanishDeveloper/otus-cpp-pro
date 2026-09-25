#pragma once

#include "document.hpp"
#include "shapes.hpp"
#include <cstdint>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

namespace editor
{
    class IDocumentSerializer
    {
    public:
        virtual ~IDocumentSerializer() = default;
        virtual void save(const Document& doc, const std::string& path) = 0;
        virtual std::unique_ptr<Document> load(const std::string& path) = 0;
    };

    class TextDocumentSerializer final : public IDocumentSerializer
    {
    public:
        void save(const Document& doc, const std::string& path) override
        {
            std::ofstream file(path);
            if (!file)
                throw std::runtime_error("Cannot write: " + path);

            for (const auto& s : doc.shapes())
            {
                const auto& c = s->color();
                file << static_cast<int>(s->type()) << ' ' << static_cast<int>(c.r) << ' '
                     << static_cast<int>(c.g) << ' ' << static_cast<int>(c.b) << ' ' << static_cast<int>(c.a) << '\n';
            }
        }

        std::unique_ptr<Document> load(const std::string& path) override
        {
            std::ifstream file(path);
            if (!file)
                throw std::runtime_error("Cannot read: " + path);

            auto doc = std::make_unique<Document>();

            int type = 0, r = 0, g = 0, b = 0, a = 0;
            while (file >> type >> r >> g >> b >> a)
            {
                auto shape = makeShape(static_cast<ShapeType>(type));

                if (!shape) continue;

                shape->setColor(Color{
                    static_cast<std::uint8_t>(r),
                    static_cast<std::uint8_t>(g),
                    static_cast<std::uint8_t>(b),
                    static_cast<std::uint8_t>(a)
                });

                doc->addShape(std::move(shape));
            }
            return doc;
        }
    };
}
