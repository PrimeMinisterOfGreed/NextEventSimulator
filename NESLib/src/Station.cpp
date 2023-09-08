#include "Station.hpp"
#include "DataProvider.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include <sstream>

void Station::ProcessArrival(Event &evt)
{
    _logger->TraceInformation("Processing Arrival of event:{}", evt.Name);
    _arrivals++;
    _sysClients++;
    _lastArrival = evt.ArrivalTime;
    if (_sysClients > _maxClients)
        _maxClients = _sysClients;
}

void Station::ProcessDeparture(Event &evt)
{
    _logger->TraceInformation("Processing Departure of event:{}", evt.Name);
    _sysClients--;
    _completions++;
}

void Station::ProcessEnd(Event &evt)
{
    _observationPeriod = _oldclock;
}

void Station::ProcessProbe(Event &evt)
{
}

void Station::Process(Event &event)
{
    _clock = event.OccurTime;
    _logger->TraceInformation("Station:{}, Processing event:{} with occur time: {}", _name, event.Name,
                              event.OccurTime);
    double interval = _clock - _oldclock;
    _oldclock = event.OccurTime;
    if (_sysClients > 0)
    {
        _busyTime += interval;
        _areaN += _sysClients * interval;
        _areaS += (_sysClients - 1) * interval;
    }
    switch (event.Type)
    {
    case EventType::ARRIVAL:
        ProcessArrival(event);
        break;
    case EventType::DEPARTURE:
        ProcessDeparture(event);
        break;
    case EventType::NO_EVENT:
        break;
    case EventType::END:
        ProcessEnd(event);
        break;
    case EventType::PROBE:
        ProcessProbe(event);
    case EventType::MAINTENANCE:
        ProcessMaintenance(event);
        break;
    }
}

void Station::ProcessMaintenance(Event &evt)
{
}

std::vector<Measure<double>> Station::GetMeasures()
{

    result.timestamp = _clock;
    result.avgInterArrival = _oldclock / _arrivals;                /* Average inter-arrival time */
    result.avgServiceTime = _busyTime / _completions;              /* Average service time */
    result.avgDelay = _areaS / _completions;                       /* Average delay time */
    result.avgWaiting = _areaN / _completions;                     /* Average wait time */
    result.utilization = _busyTime / _observationPeriod;           /* Utilization */
    result.throughput = _completions / _observationPeriod;         /* Throughput */
    result.inputRate = _arrivals / _oldclock;                      /* Input rate */
    result.arrivalRate = _arrivals / _observationPeriod;           /* Arriva rate */
    result.serviceRate = _completions / _busyTime;                 /* Service rate */
    result.traffic = _busyTime / _lastArrival;                     /* Traffic intensity */
    result.meanCustomInQueue = _areaS / _observationPeriod;        /* Mean number of customers in queue */
    result.meanCustomerInService = _busyTime / _observationPeriod; /* Mean number of customers in service */
    result.meanCustomerInSystem = _areaS / _observationPeriod;     /* Mean number of customers in system */
    return result;
}

StationStatistic Station::GetStatistics()
{
    StationStatistic result{};
    result.timestamp = _clock;
    result.avgInterArrival = _oldclock / _arrivals; /* Average inter-arrival time */

    result.avgServiceTime = _busyTime / _completions;              /* Average service time */
    result.avgDelay = _areaS / _completions;                       /* Average delay time */
    result.avgWaiting = _areaN / _completions;                     /* Average wait time */
    result.utilization = _busyTime / _observationPeriod;           /* Utilization */
    result.throughput = _completions / _observationPeriod;         /* Throughput */
    result.inputRate = _arrivals / _oldclock;                      /* Input rate */
    result.arrivalRate = _arrivals / _observationPeriod;           /* Arriva rate */
    result.serviceRate = _completions / _busyTime;                 /* Service rate */
    result.traffic = _busyTime / _lastArrival;                     /* Traffic intensity */
    result.meanCustomInQueue = _areaS / _observationPeriod;        /* Mean number of customers in queue */
    result.meanCustomerInService = _busyTime / _observationPeriod; /* Mean number of customers in service */
    result.meanCustomerInSystem = _areaS / _observationPeriod;     /* Mean number of customers in system */
    return result;
}

void Station::Reset()
{
    _arrivals = 0;
    _completions = 0;
    _sysClients = 0;
    _maxClients = 0;
    _busyTime = 0.0;
    _observationPeriod = 0.0;
    _lastArrival = 0.0;
    _areaN = 0.0;
    _areaS = 0.0;
    _oldclock = 0.0;
    _clock = 0.0;
}

Station::Station(std::string name, int station) : _name(name), _stationIndex(station), collector{name}
{
}

void Station::Initialize()
{
}
