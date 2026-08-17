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

        for (auto ip : ip_pool) {
            print_ip(ip, std::cout);
            std::cout << '\n';
        }
        auto res1 = filter(ip_pool, 1);
        for (auto ip : res1) {
            print_ip(ip, std::cout);
            std::cout << '\n';
        }

        auto res2 = filter(ip_pool, 46, 70);
        for (auto ip : res2) {
            print_ip(ip, std::cout);
            std::cout << '\n';
        }
        
        auto res3 = filter_any(ip_pool, 46);
        for (auto ip : res3) {
            print_ip(ip, std::cout);
            std::cout << '\n';
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

