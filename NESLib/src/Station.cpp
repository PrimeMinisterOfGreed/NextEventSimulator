#include "Station.hpp"
#include "DataCollector.hpp"
#include "DataProvider.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include <sstream>

enum Measures
{
    Completions,
    Arrivals,
    sysclients,
    maxclients,
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

    Update();
    DataWriter::Instance().WriteLine(collector.Csv());
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
    collector.AddMeasure(new Measure<double>{"completitions", "unit"});
    collector.AddMeasure(new Measure<double>{"arrivals", "unit"});
    collector.AddMeasure(new Measure<double>{"sysClients", "unit"});
    collector.AddMeasure(new Measure<double>{"maxClients", "unit"});
    collector.AddMeasure(new Accumulator<double>{"avgInterArrival", "ms"});
    collector.AddMeasure(new Accumulator<double>{"avgServiceTime", "ms"});
    collector.AddMeasure(new Accumulator<double>{"avgDelay", "ms"});
    collector.AddMeasure(new Accumulator<double>{"avgWaiting", "ms"});
    collector.AddMeasure(new Accumulator<double>{"utilization", ""});
    collector.AddMeasure(new Accumulator<double>{"throughput", "job/ms"});
    collector.AddMeasure(new Accumulator<double>{"inputRate", ""});
    collector.AddMeasure(new Accumulator<double>{"arrivalRate", ""});
    collector.AddMeasure(new Accumulator<double>{"serviceRate", ""});
    collector.AddMeasure(new Accumulator<double>{"traffic", ""});
    collector.AddMeasure(new Accumulator<double>{"meanCustomerInQueue", "unit"});
    collector.AddMeasure(new Accumulator<double>{"meanCustomerInService", ""});
    collector.AddMeasure(new Accumulator<double>{"meanCustomerInSystem", ""});
}

void Station::Initialize()
{
}

void Station::Update()
{
    collector.lastTimeStamp = _oldclock;
    collector[Completions](_completions);
    collector[Arrivals](_arrivals);
    collector[sysclients](_sysClients);
    collector[maxclients](_maxClients);
    collector[avgInterArrival](_oldclock / _arrivals);                /* Average inter-arrival time */
    collector[avgServiceTime](_busyTime / _completions);              /* Average service time */
    collector[avgDelay](_areaS / _completions);                       /* Average delay time */
    collector[avgWaiting](_areaN / _completions);                     /* Average wait time */
    collector[utilization](_busyTime / _observationPeriod);           /* Utilization */
    collector[throughput](_completions / _observationPeriod);         /* Throughput */
    collector[inputRate](_arrivals / _oldclock);                      /* Input rate */
    collector[arrivalRate](_arrivals / _observationPeriod);           /* Arriva rate */
    collector[serviceRate](_completions / _busyTime);                 /* Service rate */
    collector[traffic](_busyTime / _lastArrival);                     /* Traffic intensity */
    collector[meanCustomerInQueue](_areaS / _observationPeriod);      /* Mean number of customers in queue */
    collector[meanCustomerInService](_busyTime / _observationPeriod); /* Mean number of customers in service */
    collector[meanCustomerInSystem](_areaS / _observationPeriod);     /* Mean number of customers in system */
    collector._samples++;
}

DataCollector Station::Data()
{
    return collector;
}
