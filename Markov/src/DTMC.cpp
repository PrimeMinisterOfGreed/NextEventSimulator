#include "DTMC.hpp"
#include <vector>
std::vector<double> vectorMethod(Matrix<double> &dtmc, double threshold)
{
    std::vector<double> res{};
    res.resize(dtmc.Columns());
    auto old = res;
    res[0] = 1;
    while ((res - old)[0] > threshold)
    {
        old = res;
        res = res * dtmc;
    }

    return res;
}
