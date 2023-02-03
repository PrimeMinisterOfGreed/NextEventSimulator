//
// Created by drfaust on 03/02/23.
//

#pragma once
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include "EventHandler.hpp"

template<typename T>
class Measure
{
   using Accumulator = boost::accumulators::accumulator_set<T,
   boost::accumulators::features<
   boost::accumulators::tag::mean,
   boost::accumulators::tag::count,
   boost::accumulators::tag::variance,
   boost::accumulators::tag::sum
   >>;

private:
    std::string _name;
    std::string _unit;

public:
    virtual void Accumulate(T value);
    virtual std::string ToString();

};