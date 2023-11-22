#include "Collections/Matrix.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(TestMatrix, test_mat_insert)
{
    std::vector<int> a{};
    Matrix<int> m{100};
    m(0, 1) = 1;
    ASSERT_EQ(1, m(0, 1));
}