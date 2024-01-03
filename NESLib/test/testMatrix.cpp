#include "Collections/Matrix.hpp"
#include "Mva.hpp"
#include <cmath>
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

TEST(TestMatrix, test_mva)
{
    // 0.100000,10.000000,5.500000,3.500000,
    Matrix<double> routings({
        {0, 1, 0, 0}, 
        {0.10, 0, 0.55, 0.35}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}
        });
    auto visits = RouteToVisit(routings);
    double epsilon = 0.001;
    ASSERT_LE(epsilon, visits[0]-0.10);
    ASSERT_LE(epsilon, visits[1]-10.0);
    ASSERT_LE(epsilon, visits[2]-5.5);
    ASSERT_LE(epsilon, visits[3]-3.5);
}
