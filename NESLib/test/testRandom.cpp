#include "Measure.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <cmath>
#include <cstdio>
#include <fmt/core.h>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <stdio.h>
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

// https://rossetti.github.io/RossettiArenaBook/app-rnrv-rvs.html#AppRNRV:subsec:MTSRV
TEST(TestRandom, test_composition)
{
    RandomStream::Global().PlantSeeds(123456789);
    CompositionStream stream{0,
                             {0.95, 0.05},
                             [](RandomStream &rng) { return Exponential(10); },
                             [](RandomStream &rng) { return Exponential(19010); }};
    Accumulator<> _mean{"mean", ""};
    for (int i = 0; i < 100000; i++)
    {
        _mean(stream());
    }
    printf("Mean: %lf", _mean.mean());
}

TEST(TestRandom, test_routing)
{
    RandomStream::Global().PlantSeeds(123456789);
    Router router(2,{0.33,0.33,0.34},{0,1,2});
    std::vector<double> hits{0,0,0};
    for(int i = 0; i < 1000000; i++)
        hits[router()]++;
    hits.push_back(hits[0]/hits[1]);
    hits.push_back(hits[1]/hits[2]);
    hits.push_back(hits[2]/hits[0]);
    fmt::print("Hits: {}",fmt::join(hits,","));
}