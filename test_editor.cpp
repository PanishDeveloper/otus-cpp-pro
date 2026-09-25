#include <gtest/gtest.h>

#include "document.hpp"
#include "editor_controller.hpp"
#include "editor_view.hpp"
#include "serializer.hpp"
#include "shapes.hpp"

#include <cstdio>
#include <string>

using namespace editor;

namespace
{
    // Fixture for controller tests
    class ControllerTest : public  ::testing::Test
    {
    protected:
        Document doc;
        EditorView view;

        std::unique_ptr<EditorController> controller;

        void SetUp() override
        {
            controller = std::make_unique<EditorController>(doc, view);
            controller->setSerializer(std::make_shared<TextDocumentSerializer>());
        }
    };
}

// 1/ Creating a new document
TEST_F(ControllerTest, NewDocumentClearShapes)
{
    controller->createShape(ShapeType::Line);
    controller->createShape(ShapeType::Circle);
    controller->newDocument();
    EXPECT_TRUE(doc.empty());
}

// 2. Importing a document from a file
TEST_F(ControllerTest, ImportReplacesDocument)
{
    controller->createShape(ShapeType::Line);
    controller->createShape(ShapeType::Circle);
    controller->exportDocument("test_import.txt");

    controller->newDocument();
    EXPECT_TRUE(doc.empty());

    controller->importDocument("test_import.txt");
    EXPECT_EQ(doc.size(), 2u);

    std::remove("test_import.txt");
}

// 3. Exporting a document to a file
TEST_F(ControllerTest, ExportDoesNotThrow)
{
    controller->createShape(ShapeType::Rectangle);
    EXPECT_NO_THROW(controller->exportDocument("dummy.json"));
    std::remove("dummy.json");
}

// 4. Creating a graphical primitive
TEST_F(ControllerTest, CreateShapeAddsToDocument)
{
    const auto id = controller->createShape(ShapeType::Circle);
    EXPECT_NE(id, 0u);
    EXPECT_EQ(doc.size(), 1u);
}

// 5. Removal of a graphical primitive
TEST_F(ControllerTest, DeleteShapeRemovesFromDocument)
{
    const auto id = controller->createShape(ShapeType::Line);
    controller->deleteShape(id);
    EXPECT_TRUE(doc.empty());
}

// Unique ID
TEST(DocumentTest, AddShapeAssignsUniqueIds)
{
    Document doc;
    const auto id1 = doc.addShape(std::make_unique<Line>());
    const auto id2 = doc.addShape(std::make_unique<Rectangle>());
    EXPECT_NE(id1, id2);
}

// Polymorphism: clone
TEST(ShapeTest, CloneProducesCopy)
{
    Line original;
    original.setID(42);
    auto copy = original.clone();
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(copy->id(), 42u);
    EXPECT_NE(copy.get(), &original);
}

//findShape finds by id
TEST_F(ControllerTest, CreateShapeIsSearchableByID)
{
    const auto id = controller->createShape(ShapeType::Circle);
    const Shape* found = doc.findShape(id);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->id(), id);
    EXPECT_EQ(found->type(), ShapeType::Circle);
}

// selectionShape sets the flag
TEST_F(ControllerTest, SelectShapeMarksIt)
{
    const auto id = controller->createShape(ShapeType::Circle);
    controller->selectShape(id);
    EXPECT_TRUE(doc.findShape(id)->isSelected());
}

// clearSelection resets the flags for everyone
TEST_F(ControllerTest, ClearSelectionUnsetsAll)
{
    const auto id1 = controller->createShape(ShapeType::Circle);
    const auto id2 = controller->createShape(ShapeType::Line);
    controller->selectShape(id1);
    controller->selectShape(id2);
    controller->clearSelection();

    EXPECT_FALSE(doc.findShape(id1)->isSelected());
    EXPECT_FALSE(doc.findShape(id2)->isSelected());
}

TEST_F (ControllerTest, ExportImportRoundTrip)
{
    const std::string path = "test_roundtrip.txt";

    controller->createShape(ShapeType::Line);
    controller->createShape(ShapeType::Circle);
    controller->createShape(ShapeType::Rectangle);
    EXPECT_EQ(doc.size(), 3u);

    // Export
    controller->exportDocument(path);

    // Cleaning the document
    controller->newDocument();
    EXPECT_TRUE(doc.empty());

    // Import
    controller->importDocument(path);
    EXPECT_EQ(doc.size(), 3u);

    // We check the types
    EXPECT_EQ(doc.shapes()[0]->type(), ShapeType::Line);
    EXPECT_EQ(doc.shapes()[1]->type(), ShapeType::Circle);
    EXPECT_EQ(doc.shapes()[2]->type(), ShapeType::Rectangle);

    // We delete the temporary file
    std::remove(path.c_str());
}