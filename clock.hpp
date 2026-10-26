#pragma once

#include <ctime>

namespace bulk
{
    class Clock
    {
    public:
        virtual ~Clock() = default;
        [[nodiscard]] virtual std::time_t now() const = 0;
    };

    class SystemClock final : public Clock
    {
    public:
        [[nodiscard]] std::time_t now() const override
        {
            return std::time(nullptr);
        }
    };
}
