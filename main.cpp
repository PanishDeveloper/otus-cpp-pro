#include "matrix.hpp"

#include <iostream>
#include <tuple>

int main()
{
    sparse::Matrix<int, 0> matrix;

    for (int i = 0; i <= 9; ++i) matrix[i][i] = i;
    for (int i = 0; i <= 9; ++i) matrix[i][9 - i] = 9 - i;

    for (int i = 1; i <= 8; ++i)
    {
        for (int j = 1; j <= 8; ++j)
        {
            std::cout << matrix[i][j];
            if (j < 8) std::cout << ' ';
        }
        std::cout << "\n";
    }

    std::cout << "\n" << matrix.size() << "\n\n";

    for (auto c : matrix)
    {
        auto [x, y, v] = c;
        std::cout << x << ' ' << y << ' ' << v << "\n";
    }
}