#include "DTMC.hpp"
#include <algorithm>
#include <vector>
std::vector<double> vectorMethod(Matrix<double> &dtmc, double threshold)
{
    std::vector<double> res{};
    res.resize(dtmc.Columns());
    auto old = res;
    res[0] = 1;
    do
    {
        old = res;
        res = res * dtmc;
    } while (All(old - res, [threshold](double &t) { return std::abs(t) > threshold; }));

    return res;
}

DTMC::Result DTMC::Validate()
{
    {
        auto &data = Base::_data;
        if (data.size() != data[0].size())
        {
            return Result::NO_SQUARE;
        }

        for (auto &row : data)
        {
            double sum{};
            for (auto &elem : row)
            {
                sum += elem;
            }
            if (sum != 1.0)
                return Result::INVALID_SUM;
        }

        for (int i = 0; i < data.size(); i++)
        {
            auto &vec = data[i];
            bool fail = true;
            for (int j = 0; j < vec.size(); j++)
            {
                if (i != j && vec[i] > 0.0)
                    fail = false;
            }
            if (fail)
                return Result::ABSORBING_STATES;
        }
        return Result::OK;
    }
}
