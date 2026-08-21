#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <ostream>
#include <array>
#include <algorithm>

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

    uint32_t prefix = 0;
    for (size_t i = 0; i < N; ++i) {
        prefix |= (static_cast<uint32_t>(pattern[i]) << (8 * (3 - i)));
    }
    uint32_t low = prefix;
    uint32_t high = prefix | (0xFFFFFFFF >> (8 * N));

    auto first = std::lower_bound(pool.begin(), pool.end(), high,
        [](uint32_t a, uint32_t b) { return a > b; });
    auto last = std::upper_bound(pool.begin(), pool.end(), low,
        [](uint32_t a, uint32_t b) { return a > b; });

    std::vector<uint32_t> result;
    result.reserve(std::distance(first, last));
    std::copy(first, last, std::back_inserter(result));
    return result;
}
std::vector<uint32_t> filter_any(const std::vector<uint32_t>& pool, uint8_t target);