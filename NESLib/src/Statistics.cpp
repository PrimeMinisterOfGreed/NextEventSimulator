#include "Statistics.hpp"
#include <boost/math/distributions.hpp>

void StatisticCollector::Accumulate(const StationStatistic &stat)
{
    _samples++;
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

int StatisticCollector::getSamples() const
{
    return _samples;
}

double idfStudent(double df, double quantile)
{
    boost::math::students_t stud{df};
    return
            boost::math::quantile(stud, quantile
            );
}

Interval GetValueConfidence(int samples, double confidence, double mean, double variance)
{
    double delta =
            idfStudent(samples - 1, 0.95) * (variance/ sqrt(samples - 1));
    return Interval{mean-delta,mean+delta};
}