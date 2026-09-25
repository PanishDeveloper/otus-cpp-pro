#include "document.hpp"
#include "editor_view.hpp"
#include "editor_controller.hpp"
#include "serializer.hpp"

#include <iostream>
#include <memory>

namespace
{
    // Global
    editor::Document g_document;
    editor::EditorView g_view;
    editor::EditorController g_controller(g_document, g_view);

    // GUI handler
    void onNewDocumentClicked() { g_controller.newDocument(); }
    void onImportClicked(const std::string& path) { g_controller.importDocument(path); }
    void onExportClicked(const std::string& path) { g_controller.exportDocument(path); }
    void onCreateShapeClicked(editor::ShapeType type) { g_controller.createShape(type); }
    void onDeleteShapeClicked(editor::ShapeID id) { g_controller.deleteShape(id); }
    void onSelectShapeClicked(editor::ShapeID id) { g_controller.selectShape(id); }
    void onClearSelectionClicked() { g_controller.clearSelection(); }

    void printSize(const char* tag)
    {
        std::cout << "[main] " << tag << ": " << g_document.size() << " shapes\n";
    }
}

int main()
{
    g_controller.setSerializer(std::make_shared<editor::TextDocumentSerializer>());

    // 1. Creating a new document
    onNewDocumentClicked();
    printSize("after new");

    // 2. Creation of graphic primitives
    onCreateShapeClicked(editor::ShapeType::Line);
    onCreateShapeClicked(editor::ShapeType::Rectangle);
    onCreateShapeClicked(editor::ShapeType::Circle);
    printSize("after create");

    //
    onSelectShapeClicked(2);
    onClearSelectionClicked();

    // 3. Exporting a document to a file
    onExportClicked("demo.txt");

    // 4. Clean and import back — a round‑trip demonstration.
    onNewDocumentClicked();
    printSize("after clear");

    onImportClicked("demo.txt");
    printSize("after import");

    // 5. Deletion of a graphical primitive
    onDeleteShapeClicked(1);
    printSize("after delete");
}