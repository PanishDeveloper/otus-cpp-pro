#include <gtest/gtest.h>

#include "matrix.hpp"
#include <tuple>
#include <stdexcept>

TEST(MatrixTest, EmptyHasZeroSize)
{
    sparse::Matrix<int, -1> matrix;
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_TRUE(matrix.empty());
}

TEST(MatrixTest, ReadDoesNotOccupy)
{
    sparse::Matrix<int, -1> matrix;
    EXPECT_EQ(matrix.size(), 0);

    int a = matrix[0][0];
    EXPECT_EQ(a, -1);
    EXPECT_EQ(matrix.size(), 0);
}

TEST(MatrixTest, CompareWithInt)
{
    sparse::Matrix<int, -1> matrix;

    EXPECT_EQ(matrix[0][0], -1);
    EXPECT_EQ(matrix[100][100], -1);
    EXPECT_EQ(matrix.size(), 0);
}

TEST(MatrixTest, WriteStoresValue)
{
    sparse::Matrix<int, -1> matrix;
    matrix[100][100] = 314;

    EXPECT_EQ(matrix[100][100], 314);
    EXPECT_EQ(matrix.size(), 1);
}

TEST(MatrixTest, WriteDefaultFreesCell)
{
    sparse::Matrix<int, -1> matrix;
    matrix[5][5] = 42;

    EXPECT_EQ(matrix.size(), 1);
    matrix[5][5] = -1;
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(matrix[5][5], -1);
}

TEST(MatrixTest, CopyCellValue)
{
    sparse::Matrix<int, -1> matrix;
    matrix[1][1] = 5;
    matrix[2][2] = matrix[1][1];

    EXPECT_EQ(matrix[1][1], 5);
    EXPECT_EQ(matrix[2][2], 5);
    EXPECT_EQ(matrix.size(), 2);
}

TEST(MatrixTest, IterateSingleCell)
{
    sparse::Matrix<int, -1> matrix;
    matrix[100][100] = 314;

    int count = 0;
    for (auto c : matrix)
    {
        int x, y, v;
        std::tie(x, y, v) = c;
        EXPECT_EQ(x, 100);
        EXPECT_EQ(y, 100);
        EXPECT_EQ(v, 314);
        ++count;
    }

    EXPECT_EQ(count, 1);
}

TEST(MatrixTest, IteratorDereference)
{
    sparse::Matrix<int, -1> matrix;
    matrix[1][1] = 5;

    const auto it = matrix.begin();
    auto [x, y, v] = *it;

    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 1);
    EXPECT_EQ(v, 5);
}

namespace
{
    void eraseUnderIterator(sparse::Matrix<int, 0>& matrix)
    {
        for (auto c : matrix)
        {
            auto [x, y, v] = c;
            matrix[x][y] = 0;
        }
    }
}

TEST(MatrixTest, IteratorThrowsIfErased)
{
    sparse::Matrix<int, 0> matrix;
    matrix[1][1] = 5;
    matrix[2][2] = 6;
    matrix[3][3] = 7;

    EXPECT_THROW(eraseUnderIterator(matrix), std::logic_error);
}

namespace
{
    int writeFreeUnderIterator(sparse::Matrix<int, 0>& matrix)
    {
        int count = 0;
        for ([[maybe_unused]] auto c : matrix)
        {
            matrix[100][100] = 0;
            ++count;
        }
        return count;
    }
}

TEST(MatrixTest, IteratorAllowsFreeWrite)
{
    sparse::Matrix<int, 0> matrix;
    matrix[1][1] = 5;
    matrix[2][2] = 6;
    matrix[3][3] = 7;

    int count = 0;
    EXPECT_NO_THROW(count = writeFreeUnderIterator(matrix));
    EXPECT_EQ(count, 3);
}

TEST(MatrixTest, DiagonalsGive18)
{
    sparse::Matrix<int, 0> matrix;
    for (int i = 0; i <= 9; ++i) matrix[i][i] = i;
    for (int i = 0; i <= 9; ++i) matrix[i][9 - i] = 9 - i;

    EXPECT_EQ(matrix.size(), 18u);
}

namespace
{
    void postfixIncrementAfterErase(sparse::Matrix<int, 0>& matrix)
    {
        auto it = matrix.begin();
        matrix[1][1] = 0;
        it++; // NOLINT — тестируем именно постфикс
    }
}

TEST(MatrixTest, PostfixThrowsIfErased)
{
    sparse::Matrix<int, 0> matrix;
    matrix[1][1] = 5;
    matrix[2][2] = 6;
    matrix[3][3] = 7;

    EXPECT_THROW(postfixIncrementAfterErase(matrix), std::logic_error);
}