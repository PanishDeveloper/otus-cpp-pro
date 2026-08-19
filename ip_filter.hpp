#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <ostream>
#include <array>

std::vector<std::string> split(const std::string& str, char d);
uint32_t parse_ip(const std::string& ip_str);
void print_ip(uint32_t ip, std::ostream& out);

template<typename... Bytes>
std::vector<uint32_t> filter(const std::vector<uint32_t>& pool, Bytes... bytes)
{
    static_assert((std::is_integral_v<Bytes> && ...), "All arguments must be integral types");
    constexpr size_t N = sizeof...(bytes);
    static_assert(N >= 1 && N <= 4, "Number of bytes must be between 1 and 4");

    std::array<uint8_t, N> pattern = { static_cast<uint8_t>(bytes)... };
    std::vector<uint32_t> result;
    result.reserve(pool.size());

    for (uint32_t ip : pool)
    {
        bool match = true;
        for (size_t i = 0; i < N; ++i)
        {
            uint8_t byte = (ip >> (8 * (3 - i))) & 0xFF;
            if (byte != pattern[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            result.push_back(ip);
    }
    return result;
}
std::vector<uint32_t> filter_any(const std::vector<uint32_t>& pool, uint8_t target);