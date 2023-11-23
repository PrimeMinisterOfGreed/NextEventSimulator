#include "Collections/Matrix.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <vector>

TEST(TestMatrix, test_mat_insert)
{
    std::vector<int> a{};
    Matrix<int> m{100};
    m(0, 1) = 1;
    ASSERT_EQ(1, m(0, 1));
}

TEST(TestMatrix, test_mat_instantiation)
{
    Matrix<int> a({{1, 2, 3}, {2, 3, 4}, {5, 6, 7}});
    auto p = a.ToString();
    printf("%s", p.c_str());
}

TEST(TestMatrix, test_mul)
{
    Matrix<double> a({{2, 2, 2}, {2, 2, 2}, {2, 2, 2}});
    auto b = a * a;
    printf("%s", b.ToString().c_str());
}

TEST(TestMatrix, test_left_mul)
{
    Matrix<double> a({{2, 2, 2}, {2, 2, 2}, {2, 2, 2}});
    std::vector<double> b = {2, 2, 2};
    auto c = b * a;
    ASSERT_EQ(12, c[0]);
}
