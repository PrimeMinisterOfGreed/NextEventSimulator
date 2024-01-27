//
// Created by drfaust on 03/02/23.
//

#include "Measure.hpp"
#include "Core.hpp"
#include <cmath>
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

void CovariatedMeasure::Accumulate(double value, double time)
{
    _count++;
    _current[0] = value;
    _current[1] = time;
    _sum[0] += value;
    _sum[1] += pow(value, 2);
    _times[0] += time;
    _times[1] += pow(time, 2);
    _weightedsum += (value * time);
}

double CovariatedMeasure::mean()
{
    return _sum[0]/_times[0];
}

double CovariatedMeasure::variance()
{
    double r = _sum[0] / _times[0];
    double a = _sum[1] - (2 * r * _weightedsum) + (pow(r, 2) * _times[1]);
    return a / (_count - 1);
}

Interval CovariatedMeasure::confidence()
{
    double r = _sum[0] / _times[0];
    double a = sqrt(((double)_count / (_count - 1.0)));
    double b = (sqrt(_sum[1] - (2 * r * _weightedsum) + (pow(r, 2) * _times[1])));
    double delta = a * (b / _times[0]);
    return Interval(mean(), delta);
}
