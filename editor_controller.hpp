#pragma once

#include "document.hpp"
#include "shapes.hpp"
#include "serializer.hpp"
#include "editor_view.hpp"
#include <memory>
#include <string>

namespace editor
{
    class EditorController
    {
    public:
        EditorController(Document& doc, EditorView& view) : m_doc(doc), m_view(view) {}

        void newDocument()
        {
            m_doc.clear();
            m_view.showMessage("new document created");
        }

        void importDocument(const std::string& path)
        {
            if (!m_serializer)
            {
                m_view.showMessage("no serializer set");
                return;
            }
            try
            {
                if (auto loaded = m_serializer->load(path))
                {
                    m_doc.clear();
                    for (const auto& s : loaded->shapes())
                        m_doc.addShape(s->clone());
                }
                m_view.showMessage("imported: " + path);
            }
            catch (const std::exception& e)
            {
                m_view.showMessage(std::string("import failed: ") + e.what());
            }
        }

        void exportDocument(const std::string& path)
        {
            if (!m_serializer)
            {
                m_view.showMessage("no serializer set");
                return;
            }
            try
            {
                m_serializer->save(m_doc, path);
                m_view.showMessage("exported: " + path);
            }
            catch (const std::exception& e)
            {
                m_view.showMessage(std::string("export failed: ") + e.what());
            }
        }

        ShapeID createShape(ShapeType type)
        {
            return m_doc.addShape(makeShape(type));
        }

        void deleteShape(ShapeID id)
        {
            if (m_doc.removeShape(id))
                m_view.showMessage("shape deleted");
        }

        void selectShape(ShapeID id)
        {
            if (auto* s = m_doc.findShape(id))
            {
                s->setSelected(true);
                m_view.showMessage("shape selected");
            }
            else
            {
                m_view.showMessage("shape not found");
            }
        }

        void clearSelection()
        {
            for (const auto& s : m_doc.shapes())
                s->setSelected(false);

            m_view.showMessage("selection cleared");
        }

        void setSerializer(std::shared_ptr<IDocumentSerializer> serializer)
        {
            m_serializer = std::move(serializer);
        }

    private:
        Document& m_doc;
        EditorView& m_view;

        std::shared_ptr<IDocumentSerializer> m_serializer;
    };
}