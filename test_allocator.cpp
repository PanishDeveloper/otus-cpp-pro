#include <gtest/gtest.h>
#include <map>
#include "fixed_allocator.hpp"
#include "fixed_list.hpp"


TEST(FixedAllocatorTest, AllocateUpToN)
{
    FixedAllocator<int, 5> alloc;
    int* p1 = alloc.allocate(3);
    int* p2 = alloc.allocate(2);
    EXPECT_NE(p1, p2);
}

TEST(FixedAllocatorTest, AllocateMoreThanNThrows)
{
    FixedAllocator<int, 3> alloc;
    alloc.allocate(3);
    EXPECT_THROW(alloc.allocate(1), std::bad_alloc);
}

TEST(FixedAllocatorTest, AllocateZeroReturnsNull)
{
    FixedAllocator<int, 5> alloc;
    int* p = alloc.allocate(0);
    EXPECT_EQ(p, nullptr);
}

TEST(FixedAllocatorTest, RebindWorks)
{
    using IntAlloc = FixedAllocator<int, 10>;
    using PairAlloc = typename IntAlloc::template rebind<std::pair<const int, int>>::other;
    PairAlloc pairAlloc;
    auto* p = pairAlloc.allocate(5);
    EXPECT_NE(p, nullptr);
    pairAlloc.deallocate(p, 5);
}

TEST(FixedAllocatorTest, CopyConstructible)
{
    FixedAllocator<int, 10> alloc1;
    FixedAllocator<int, 10> alloc2 = alloc1;
    int* p = alloc2.allocate(5);
    EXPECT_NE(p, nullptr);
}

TEST(FixedListTest, PushBackAndIteration)
{
    FixedList<int> list;
    for (int i = 0; i < 5; ++i)
    {
        list.push_back(i);
    }
    int expected = 0;
    for (int val : list)
    {
        EXPECT_EQ(val, expected++);
    }
    EXPECT_EQ(expected, 5);
}

TEST(FixedListTest, SizeAndEmpty)
{
    FixedList<int> list;
    EXPECT_TRUE(list.empty());
    list.push_back(42);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
}

TEST(FixedListTest, Clear)
{
    FixedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.begin(), list.end());
}

TEST(FixedListTest, WithCustomAllocator)
{
    using Alloc = FixedAllocator<int, 10>;
    FixedList<int, Alloc> list;
    for (int i = 0; i < 10; ++i) list.push_back(i);
    int expected = 0;
    for (int val : list) EXPECT_EQ(val, expected++);
}

TEST(MapAllocatorTest, MapWithFixedAllocator)
{
    using MapAlloc = FixedAllocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<>, MapAlloc> m;
    for (int i = 0; i < 10; ++i) m[i] = i * i;
    EXPECT_EQ(m.size(), 10);
    EXPECT_EQ(m[5], 25);
}