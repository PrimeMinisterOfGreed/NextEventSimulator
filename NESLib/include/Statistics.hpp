#pragma once
#include "Station.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <map>
using StatisticAccumulator = boost::accumulators::accumulator_set<
    double, boost::accumulators::features<boost::accumulators::tag::mean, boost::accumulators::tag::variance>>;

using Interval = std::pair<double,double>;

class StatisticCollector
{
  private:
    int _samples = 0;
public:
    int getSamples() const;

private:
    StatisticAccumulator _avgInterArrival;
    StatisticAccumulator _avgServiceTime;
    StatisticAccumulator _avgDelay;
    StatisticAccumulator _avgWaiting;
    StatisticAccumulator _utilization;
    StatisticAccumulator _throughput;
    StatisticAccumulator _inputRate;
    StatisticAccumulator _arrivalRate;
    StatisticAccumulator _serviceRate;
    StatisticAccumulator _traffic;
    StatisticAccumulator _meanCustomInQueue;
    StatisticAccumulator _meanCustomerInService;
    StatisticAccumulator _meanCustomerInSystem;


    public:

    std::map<std::string,StatisticAccumulator>& GetAccumulators();
    std::map<std::string,Interval>& GetConfidenceIntervals(double confidence);

    void Accumulate(const StationStatistic& stat);

    std::string ToString();
};

double idfStudent(double df, double quantile);

