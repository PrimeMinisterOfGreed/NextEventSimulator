#pragma once

#include "Station.hpp"
#include <map>
#include "Measure.hpp"

using Interval = std::pair<double, double>;

class StatisticCollector
{
private:
    int _samples = 0;
public:
    int getSamples() const;

private:
    Measure<double> _avgInterArrival;
    Measure<double> _avgServiceTime;
    Measure<double> _avgDelay;
    Measure<double> _avgWaiting;
    Measure<double> _utilization;
    Measure<double> _throughput;
    Measure<double> _inputRate;
    Measure<double> _arrivalRate;
    Measure<double> _serviceRate;
    Measure<double> _traffic;
    Measure<double> _meanCustomInQueue;
    Measure<double> _meanCustomerInService;
    Measure<double> _meanCustomerInSystem;

public:

    std::map<std::string, Measure<double>> &GetAccumulators();

    std::map<std::string, Interval> &GetConfidenceIntervals(double confidence);

    void Accumulate(const StationStatistic &stat);

    std::string ToString();
};

