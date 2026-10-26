#pragma once

#include "block.hpp"

#include <string>

namespace bulk
{
    // Единственная абстракция, от которой зависит процессор.
    class BlockSink
    {
    public:
        virtual ~BlockSink() = default;
        virtual void onBlock(const Block& block) = 0;
    };

    // "cmd1, cmd2, cmd3" - общее форматирование для всех синков.
    [[nodiscard]] inline std::string joinCommands(const Block& block)
    {
        std::string result;
        for (const auto& cmd : block.commands())
        {
            if (!result.empty()) result += ", ";
            result += cmd;
        }
        return result;
    }
}
