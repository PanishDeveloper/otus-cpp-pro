#pragma once

#include <cstddef>
#include <ctime>
#include <string>
#include <utility>
#include <vector>

namespace bulk
{
    class Block
    {
    public:
        Block() = default;
        void setStartedAt(std::time_t t) noexcept { m_startedAt = t; }

        void add(std::string command)
        {
            m_commands.push_back(std::move(command));
        }

        [[nodiscard]] bool empty() const noexcept { return m_commands.empty(); }
        [[nodiscard]] std::size_t size() const noexcept { return m_commands.size(); }
        [[nodiscard]] std::time_t startedAt() const noexcept { return m_startedAt; }
        [[nodiscard]] const std::vector<std::string>& commands() const noexcept { return m_commands; }

    private:
        std::vector<std::string> m_commands;
        std::time_t m_startedAt = 0;
    };
}
