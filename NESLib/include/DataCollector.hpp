#pragma once

#include "Measure.hpp"
#include "Station.hpp"
#include <map>

using Interval = std::pair<double, double>;

class DataCollector
{
  private:
    int _samples = 0;

  public:
    int getSamples() const;

  private:
    std::string _stationName;
    Measure<double> _avgInterArrival{"avgInterval", "ms"};
    Measure<double> _avgServiceTime{"avgServiceTime", "ms"};
    Measure<double> _avgDelay{"avgDelay", "ms"};
    Measure<double> _avgWaiting{"avgWaiting", "ms"};
    Measure<double> _utilization{"utilization", ""};
    Measure<double> _throughput{"throughput", "job/ms"};
    Measure<double> _inputRate{"inputRate", ""};
    Measure<double> _arrivalRate{"arrivalRate", ""};
    Measure<double> _serviceRate{"serviceRate", ""};
    Measure<double> _traffic{"traffic", ""};
    Measure<double> _meanCustomerInQueue{"meanCustomerInQueue", "unit"};
    Measure<double> _meanCustomerInService{"meanCustomerInService", ""};
    Measure<double> _meanCustomerInSystem{"meanCustomerInSystem", ""};

  public:
    std::map<std::string, Measure<double>> GetAccumulators();
    std::map<std::string, Interval> GetConfidenceIntervals(double confidence);
    std::string ToString();
    DataCollector(std::string stationName);
};
