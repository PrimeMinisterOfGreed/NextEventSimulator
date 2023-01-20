#include "Statistics.hpp"

void StatisticCollector::Accumulate(const StationStatistic &stat)
{
    _avgInterArrival(stat.avgInterArrival);
    _avgServiceTime(stat.avgServiceTime);
    _avgDelay(stat.avgDelay);
    _avgWaiting(stat.avgWaiting);
    _utilization(stat.utilization);
    _throughput(stat.throughput);
    _inputRate(stat.inputRate);
    _arrivalRate(stat.arrivalRate);
    _serviceRate(stat.serviceRate);
    _traffic(stat.traffic);
    _meanCustomInQueue(stat.meanCustomInQueue);
    _meanCustomerInService(stat.meanCustomerInService);
    _meanCustomerInSystem(stat.meanCustomerInSystem);
}
