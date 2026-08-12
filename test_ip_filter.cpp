#include "ip_filter.hpp"

#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cstdint>


namespace
{
    uint32_t make_ip(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(b) << 16) |
           (static_cast<uint32_t>(c) << 8)  |
           static_cast<uint32_t>(d);
}
}

TEST(IPParserTest, ValidAddresses)
{
    EXPECT_EQ(parse_ip("1.2.3.4"), make_ip(1,2,3,4));
    EXPECT_EQ(parse_ip("255.255.255.255"), make_ip(255,255,255,255));
    EXPECT_EQ(parse_ip("0.0.0.0"), make_ip(0,0,0,0));
    EXPECT_EQ(parse_ip("192.168.1.1"), make_ip(192,168,1,1));
}

TEST(IPParserTest, InvalidFormats)
{
    EXPECT_THROW(parse_ip("1.2.3"), std::runtime_error);
    EXPECT_THROW(parse_ip("1.2.3.4.5"), std::runtime_error);
    EXPECT_THROW(parse_ip("1.2.3.256"), std::runtime_error);
    EXPECT_THROW(parse_ip("1.2.-1.4"), std::runtime_error);
    EXPECT_THROW(parse_ip("abc.def.ghi.jkl"), std::runtime_error);
}

TEST(IPParserTest, SplitFunction)
{
    auto v = split("11.22.33.44", '.');
    ASSERT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], "11");
    EXPECT_EQ(v[1], "22");
    EXPECT_EQ(v[2], "33");
    EXPECT_EQ(v[3], "44");
}

TEST(IPParserTest, SortOrder)
{
    std::vector<uint32_t> ips {
        parse_ip("1.2.3.4"),
        parse_ip("1.10.1.1"),
        parse_ip("1.1.1.1")
    };
    std::sort(ips.begin(), ips.end(), std::greater<>());
    std::vector<uint32_t> expected {
        parse_ip("1.10.1.1"),
        parse_ip("1.2.3.4"),
        parse_ip("1.1.1.1")
    };
    EXPECT_EQ(ips, expected);
}

TEST(IPFilterTest, FilterFirstByte)
{
    std::vector<uint32_t> ips = {
        parse_ip("1.2.3.4"),
        parse_ip("46.70.1.1"),
        parse_ip("1.10.20.30"),
        parse_ip("46.70.2.2"),
        parse_ip("2.3.4.5")
    };
    std::sort(ips.begin(), ips.end(), std::greater<>());
    std::vector<uint32_t> filtered;
    for (auto ip : ips) {
        if ((ip >> 24) == 1) filtered.push_back(ip);
    }
    std::vector<uint32_t> expected = {
        parse_ip("1.10.20.30"),
        parse_ip("1.2.3.4")
    };
    EXPECT_EQ(filtered, expected);
}

TEST(IPFilterTest, FilterFirstAndSecond)
{
    std::vector<uint32_t> ips = {
        parse_ip("46.70.1.1"),
        parse_ip("46.71.1.1"),
        parse_ip("46.70.2.2"),
        parse_ip("47.70.1.1")
    };
    std::sort(ips.begin(), ips.end(), std::greater<>());
    std::vector<uint32_t> filtered;
    for (auto ip : ips) {
        if ((ip >> 24) == 46 && ((ip >> 16) & 0xFF) == 70)
            filtered.push_back(ip);
    }
    std::vector<uint32_t> expected = {
        parse_ip("46.70.2.2"),
        parse_ip("46.70.1.1")
    };
    EXPECT_EQ(filtered, expected);
}

TEST(IPFilterTest, FilterAnyByte)
{
    std::vector<uint32_t> ips = {
        parse_ip("46.1.1.1"),
        parse_ip("1.46.1.1"),
        parse_ip("1.1.46.1"),
        parse_ip("1.1.1.46"),
        parse_ip("1.1.1.1")
    };
    std::sort(ips.begin(), ips.end(), std::greater<>());
    std::vector<uint32_t> filtered;
    for (auto ip : ips) {
        if ((ip >> 24) == 46 || ((ip >> 16) & 0xFF) == 46 ||
            ((ip >> 8) & 0xFF) == 46 || (ip & 0xFF) == 46) {
            filtered.push_back(ip);
            }
    }
    std::vector<uint32_t> expected = {
        parse_ip("46.1.1.1"),
        parse_ip("1.46.1.1"),
        parse_ip("1.1.46.1"),
        parse_ip("1.1.1.46")
    };
    EXPECT_EQ(filtered, expected);
}

TEST(IPFilterTest, PrintIP)
{
    std::ostringstream oss;
    print_ip(0x0A0B0C0D, oss);
    EXPECT_EQ(oss.str(), "10.11.12.13");
}