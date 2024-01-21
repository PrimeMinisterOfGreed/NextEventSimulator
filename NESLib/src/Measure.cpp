//
// Created by drfaust on 03/02/23.
//

#include "Measure.hpp"
#include "Core.hpp"
#include <numeric>
#include <vector>

std::vector<double> slice(std::vector<double>::iterator begin, std::vector<double>::iterator end)
{
    std::vector<double> res{};
    auto itr = begin;
    while (itr != end)
    {
        res.push_back(*itr);
    }
    return res;
}

