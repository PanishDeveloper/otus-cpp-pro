#pragma once

#include "block_sink.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace bulk
{
    class FileSink final : public BlockSink
    {
    public:
        explicit FileSink(std::string prefix = "bulk") : m_prefix(std::move(prefix)) {}

        void onBlock(const Block& block) override
        {
            const std::string base = m_prefix + std::to_string(block.startedAt());
            std::string name = base + ".log";
            for (int i = 1; std::filesystem::exists(name); ++i)
                name = base + "_" + std::to_string(i) + ".log";

            std::ofstream out(name);
            if (!out) throw std::runtime_error("FileSink: cannot open " + name);
            out << joinCommands(block) << "\n";
            if (!out) throw std::runtime_error("FileSink: write failed " + name);
        }

    private:
        std::string m_prefix;
    };
}
