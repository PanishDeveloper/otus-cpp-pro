#pragma once

#include "block_sink.hpp"

#include <iostream>

namespace bulk
{
    class ConsoleSink final : public BlockSink
    {
    public:
        explicit ConsoleSink(std::ostream& out = std::cout) : m_out(out) {}

        void onBlock(const Block& block) override
        {
            m_out << "bulk: " << joinCommands(block) << std::endl;
        }

    private:
        std::ostream& m_out;
    };
}
