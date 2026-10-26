#pragma once

#include "block.hpp"
#include "clock.hpp"
#include "block_sink.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace bulk
{
    class BulkProcessor
    {
    public:
        BulkProcessor(std::size_t staticSize, const Clock& clock) : m_staticSize(staticSize), m_clock(clock) {}

        void addSink(BlockSink& sink) { m_sinks.push_back(&sink); }

        void addLine(const std::string& line)
        {
            if (line == "{")
            {
                if (m_depth == 0)
                    flush();
                ++m_depth;
                return;
            }

            if (line == "}")
            {
                if (m_depth > 0)
                {
                    --m_depth;
                    if (m_depth == 0) flush();
                }
                return;
            }

            if (m_current.empty()) m_current.setStartedAt(m_clock.now());
            m_current.add(line);

            if (m_depth == 0 && m_current.size() == m_staticSize) flush();
        }

        void finish()
        {
            if (m_depth == 0) flush();
        }

        [[nodiscard]] std::size_t failures() const noexcept { return m_failures; }
    private:
        void flush()
        {
            if (m_current.empty()) return;

            for (auto* sink : m_sinks)
            {
                try
                {
                    sink->onBlock(m_current);
                } catch (const std::exception& e)
                {
                    std::cerr << "sink failed: " << e.what() << "\n";
                    ++m_failures;
                }
            }
            m_current = Block{};
        }

        std::size_t m_staticSize;
        const Clock& m_clock;
        std::vector<BlockSink*> m_sinks;
        Block m_current;
        int m_depth = 0;
        std::size_t m_failures = 0;
    };
}
