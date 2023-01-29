#pragma once
#include "Station.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

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
    StatisticAccumulator avgInterArrival() const { return _avgInterArrival; }

    StatisticAccumulator avgServiceTime() const { return _avgServiceTime; }

    StatisticAccumulator avgDelay() const { return _avgDelay; }

    StatisticAccumulator avgWaiting() const { return _avgWaiting; }

    StatisticAccumulator utilization() const { return _utilization; }

    StatisticAccumulator throughput() const { return _throughput; }

    StatisticAccumulator inputRate() const { return _inputRate; }

    StatisticAccumulator arrivalRate() const { return _arrivalRate; }

    StatisticAccumulator serviceRate() const { return _serviceRate; }

    StatisticAccumulator traffic() const { return _traffic; }

    StatisticAccumulator meanCustomInQueue() const { return _meanCustomInQueue; }

    StatisticAccumulator meanCustomerInService() const { return _meanCustomerInService; }

    StatisticAccumulator meanCustomerInSystem() const
    {
        return _meanCustomerInSystem;
    }

    void Accumulate(const StationStatistic& stat);


};

double idfStudent(double df, double quantile);
