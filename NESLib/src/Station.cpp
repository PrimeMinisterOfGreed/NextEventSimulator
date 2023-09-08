#include "Station.hpp"
#include "DataCollector.hpp"
#include "DataProvider.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "Lazy.hpp"
#include "LogEngine.hpp"
#include <sstream>

enum Measures
{
    avgInterArrival,
    avgServiceTime,
    avgDelay,
    avgWaiting,
    utilization,
    throughput,
    inputRate,
    arrivalRate,
    serviceRate,
    traffic,
    meanCustomerInQueue,
    meanCustomerInService,
    meanCustomerInSystem,
};

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
    _logger = LogEngine::Instance();
    collector.AddMeasure(Measure<double>{"avgInterArrival", "ms"});
    collector.AddMeasure(Measure<double>{"avgServiceTime", "ms"});
    collector.AddMeasure(Measure<double>{"avgDelay", "ms"});
    collector.AddMeasure(Measure<double>{"avgWaiting", "ms"});
    collector.AddMeasure(Measure<double>{"utilization", ""});
    collector.AddMeasure(Measure<double>{"throughput", "job/ms"});
    collector.AddMeasure(Measure<double>{"inputRate", ""});
    collector.AddMeasure(Measure<double>{"arrivalRate", ""});
    collector.AddMeasure(Measure<double>{"serviceRate", ""});
    collector.AddMeasure(Measure<double>{"traffic", ""});
    collector.AddMeasure(Measure<double>{"meanCustomerInQueue", "unit"});
    collector.AddMeasure(Measure<double>{"meanCustomerInService", ""});
    collector.AddMeasure(Measure<double>{"meanCustomerInSystem", ""});
}

void Station::Initialize()
{
}

void Station::Update()
{
    collector.lastTimeStamp = _oldclock;
    collector._measures[avgInterArrival](_oldclock / _arrivals);           /* Average inter-arrival time */
    collector._measures[avgServiceTime](_busyTime / _completions);         /* Average service time */
    collector._measures[avgDelay](_areaS / _completions);                  /* Average delay time */
    collector._measures[avgWaiting](_areaN / _completions);                /* Average wait time */
    collector._measures[utilization](_busyTime / _observationPeriod);      /* Utilization */
    collector._measures[throughput](_completions / _observationPeriod);    /* Throughput */
    collector._measures[inputRate](_arrivals / _oldclock);                 /* Input rate */
    collector._measures[arrivalRate](_arrivals / _observationPeriod);      /* Arriva rate */
    collector._measures[serviceRate](_completions / _busyTime);            /* Service rate */
    collector._measures[traffic](_busyTime / _lastArrival);                /* Traffic intensity */
    collector._measures[meanCustomerInQueue](_areaS / _observationPeriod); /* Mean number of customers in queue */
    collector._measures[meanCustomerInService](_busyTime /
                                               _observationPeriod);         /* Mean number of customers in service */
    collector._measures[meanCustomerInSystem](_areaS / _observationPeriod); /* Mean number of customers in system */
}

DataCollector Station::Data()
{
    return collector;
}
