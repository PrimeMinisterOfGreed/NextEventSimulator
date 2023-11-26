#include "rngs.hpp"
#include "rvgs.h"
#include <cstdio>
#include <gtest/gtest.h>
#include <vector>

double mean(std::vector<double> &vals)
{
    double sum = vals[0];
    for (int i = 1; i < vals.size(); i++)
        sum += vals[i];
    return sum / vals.size();
}

TEST(TestRandom, test_poisson)
{
    VariableStream stream{0, [](RandomStream generator) { return Exponential(1 / 0.14); }};
    std::vector<double> values{};
    for (int i = 0; i < 1000; i++)
    {
        values.push_back(stream());
    }
    ASSERT_GE(0.1, abs(mean(values) - 1 / 0.14));
}