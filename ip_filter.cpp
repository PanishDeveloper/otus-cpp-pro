#include "ip_filter.hpp"
#include <stdexcept>
#include <cstdint>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

uint32_t parse_ip(const std::string& ip_str)
{
    auto parts = split(ip_str, '.');

    if (parts.size() != 4)
        throw std::runtime_error("Invalid IP format: " + ip_str);

    int n1, n2, n3, n4;
    try
    {
        n1 = std::stoi(parts[0]);
        n2 = std::stoi(parts[1]);
        n3 = std::stoi(parts[2]);
        n4 = std::stoi(parts[3]);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Invalid IP format: " + ip_str);
    }

    if (n1 < 0 || n1 > 255 || n2 < 0 || n2 > 255 || n3 < 0 || n3 > 255 || n4 < 0 || n4 > 255)
        throw std::runtime_error("IP octet out of range: " + ip_str);

    return (static_cast<uint32_t>(n1) << 24) |
           (static_cast<uint32_t>(n2) << 16) |
           (static_cast<uint32_t>(n3) << 8)  |
           (static_cast<uint32_t>(n4));
}

void print_ip(uint32_t ip, std::ostream& out)
{
    out << ((ip >> 24) & 0xFF) << '.' << ((ip >> 16) & 0xFF) << '.' << ((ip >> 8) & 0xFF) << '.' << (ip & 0xFF);
}

std::vector<uint32_t> filter(const std::vector<uint32_t>& pool, uint8_t byte1, int byte2)
{
    std::vector<uint32_t> result;
    result.reserve(pool.size());

    if (byte2 == -1) {
        // Фильтр только по первому байту
        for (uint32_t ip : pool) {
            if ((ip >> 24) == byte1) {
                result.push_back(ip);
            }
        }
    } else {
        auto b2 = static_cast<uint8_t>(byte2); 
        for (uint32_t ip : pool) {
            if ((ip >> 24) == byte1 && ((ip >> 16) & 0xFF) == b2) {
                result.push_back(ip);
            }
        }
    }
    return result;
}

std::vector<uint32_t> filter_any(const std::vector<uint32_t>& pool, uint8_t target)
{
    std::vector<uint32_t> result;
    result.reserve(pool.size());
    for (uint32_t ip : pool) {
        if ((ip >> 24) == target ||
            ((ip >> 16) & 0xFF) == target ||
            ((ip >> 8) & 0xFF) == target ||
            (ip & 0xFF) == target) {
            result.push_back(ip);
            }
    }
    return result;
}