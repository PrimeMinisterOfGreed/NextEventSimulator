#include "DTMC.hpp"
#include "LogEngine.hpp"
#include <cstdio>
#include <fmt/core.h>
#include <vector>

int main()
{
    Matrix<double> d({{0.80, 0.15, 0.05}, {0.70, 0.20, 0.10}, {0.50, 0.30, 0.20}});
    auto v = vectorMethod(d, 0.001);
    std::string res = "";
    for (auto &e : v)
    {
        res += makeformat("{},", e);
    }
    printf("%s", res.c_str());
}
