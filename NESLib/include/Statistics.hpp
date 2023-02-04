#pragma once

#include "Station.hpp"
#include <map>


using Interval = std::pair<double, double>;

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

    std::map<std::string, StatisticAccumulator> &GetAccumulators();

    std::map<std::string, Interval> &GetConfidenceIntervals(double confidence);

    void Accumulate(const StationStatistic &stat);

    std::string ToString();
};

double idfStudent(double df, double quantile);

class BaseVariableMonitor
{
private:
    std::map<std::string, BaseRandomVariable*> _register{};
    std::map<std::string, FunctionHandler<double>*> _handlers;
protected:
    virtual void Collect(std::string name, double value) = 0;

public:
    virtual void AddRandomVar(std::string name, BaseRandomVariable *variable);

    virtual void RemoveRandomVar(std::string name);
};



class ValueCollector : public BaseVariableMonitor
{
private:
    std::map<std::string, VariableAccumulator> _counters{};
protected:
    void Collect(std::string name, double value) override;
public:
};