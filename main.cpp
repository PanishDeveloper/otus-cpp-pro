#include "ip_filter.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

int main()
{
    try
    {
        std::vector<uint32_t> ip_pool;
        std::string line;

        while (std::getline(std::cin, line))
        {
            auto columns = split(line, '\t');
            if (columns.empty()) continue;
            ip_pool.push_back(parse_ip(columns[0]));
        }

        std::sort(ip_pool.begin(), ip_pool.end(), std::greater<>());

        // 1. Все адреса
        for (auto ip : ip_pool)
        {
            print_ip(ip, std::cout);
            std::cout <<'\n';
        }

        // 2. Первый байт == 1
        for (auto ip : ip_pool)
        {
            if ((ip >> 24) == 1)
            {
                print_ip(ip, std::cout);
                std::cout << '\n';
            }
        }

        // Первый == 46, второй == 70
        for (auto ip : ip_pool)
        {
            if ((ip >> 24) == 46 && ((ip >> 16) & 0xFF) == 70)
            {
                print_ip(ip, std::cout);
                std::cout << '\n';
            }
        }

        // Любой байт == 46
        for (auto ip : ip_pool)
        {
            if ((ip >> 24) == 46 || ((ip >> 16) & 0xFF) == 46 || ((ip >> 8) & 0xFF) == 46 || (ip & 0xFF) == 46)
            {
                print_ip(ip, std::cout);
                std::cout << '\n';
            }
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

