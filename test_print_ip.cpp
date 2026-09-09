#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include<list>
#include <tuple>
#include "print_ip.hpp"


template <typename T>
static std::string capture_output(const T& value)
{
    std::ostringstream oss;
    print_ip(oss, value);
    return oss.str();
}

// Тесты для целых чисел
TEST(PrintIP, Int8)
{
    EXPECT_EQ(capture_output(int8_t{-1}), "255\n");
}

TEST(PrintIP, Int16)
{
    EXPECT_EQ(capture_output(int16_t{0}), "0.0\n");
}

TEST(PrintIP, Int32)
{
    EXPECT_EQ(capture_output(int32_t{2130706433}), "127.0.0.1\n");
}

TEST(PrintIP, Int64)
{
    EXPECT_EQ(capture_output(int64_t{8875824491850138409LL}), "123.45.67.89.101.112.131.41\n");
}

// Тесты для строк
TEST(PrintIP, String)
{
    EXPECT_EQ(capture_output(std::string{"Hello, World!"}), "Hello, World!\n");
}

// Тесты для контейнеров
TEST(PrintIP, Vector)
{
    std::vector<int> v = {100, 200, 300, 400};
    EXPECT_EQ(capture_output(v), "100.200.300.400\n");
}

TEST(PrintIP, List)
{
    std::list<short> l = {400, 300, 200, 100};
    EXPECT_EQ(capture_output(l), "400.300.200.100\n");
}

// Тесты для кортежа
TEST(PrintIP, TupleSameTypes)
{
    auto t = std::make_tuple(123, 456, 789, 0);
    EXPECT_EQ(capture_output(t), "123.456.789.0\n");
}