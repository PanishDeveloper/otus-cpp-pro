#include <iostream>
#include <map>
#include <utility>
#include "fixed_allocator.hpp"
#include "fixed_list.hpp"

static int factorial(int n)
{
    int res = 1;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

int main()
{
    // 1. map со стандартным аллокатором
    std::map<int, int> m1;
    for (int i = 0; i < 10; ++i)
    {
        m1[i] = factorial(i);
    }
    std::cout << "std::map(std::allocator):\n";
    for (const auto& p : m1)
    {
        std::cout << p.first << ". " << p.second << "\n";
    }
    std::cout << std::endl;

    // 2. map с моим аллокатором
    using mapAlloc = FixedAllocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<>, mapAlloc> m2;
    for (int i = 0; i < 10; ++i)
    {
        m2[i] = factorial(i);
    }
    std::cout << "std::map(FixedAllocator<10>:\n";
    for (const auto& p : m2)
    {
        std::cout << p.first << ". " << p.second << "\n";
    }
    std::cout << std::endl;

    // 3. FixedList со стандартным аллокатором
    FixedList<int> list1;
    for (int i = 0; i < 10; ++i)
    {
        list1.push_back(i);
    }
    std::cout << "FixedList(std::allocator):\n";
    for (int val : list1)
    {
        std::cout << val << ". ";
    }
    std::cout << "\n" << std::endl;

    // 4. FixedList с моим аллокатором (10 элементов)
    using intAlloc = FixedAllocator<int, 10>;
    FixedList<int, intAlloc> list2;
    for (int i = 0; i < 10; ++i)
    {
        list2.push_back(i);
    }
    std::cout << "FixedList(FixedAllocator<10>):\n";
    for (int val : list2)
    {
        std::cout << val << ". ";
    }
    std::cout << std::endl;
}