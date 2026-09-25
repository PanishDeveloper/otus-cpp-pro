#pragma once

#include <iostream>
#include <string>

namespace editor
{
    class Document;
    
    class EditorView
    {
    public:
        void display(const Document& doc)
        {
            // TODO: Вызвать IRenderer для отрисовки документа
            (void)doc;
            std::cout << "[view] display\n";
        }

        void showMessage(const std::string& message)
        {
            std::cout<< "[view] " << message << '\n';
        }
    };
}
