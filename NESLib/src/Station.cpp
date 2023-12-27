#include "Station.hpp"
#include "Core.hpp"
#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "Scheduler.hpp"
#include <cassert>
#include <fmt/core.h>
#include <iterator>
#include <regex>
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

void BaseStation::ProcessArrival(Event &evt)
{
    _arrivals++;
    _sysClients++;
    _lastArrival = evt.OccurTime;
    if (_sysClients > _maxClients)
        _maxClients = _sysClients;
}

void BaseStation::ProcessDeparture(Event &evt)
{
    _sysClients--;
    _completions++;
}

void BaseStation::ProcessEnd(Event &evt)
{
    _observationPeriod = _oldclock;
}

BaseStation::BaseStation(std::string name) : _logger(name), _name(name)
{
}

void BaseStation::ProcessProbe(Event &evt)
{
    _observationPeriod = _oldclock;
}

void Station::ProcessProbe(Event &evt)
{
    BaseStation::ProcessProbe(evt);
    collector.lastTimeStamp = _oldclock;
    Update();
}

void BaseStation::Process(Event &event)
{
    if (event.Type == END)
    {
        ProcessEnd(event);
        return;
    }
    if (event.OccurTime < _clock)
    {
        panic(fmt::format("Event {} occur at a lesser time of {} in station {}", event, _clock, _name));
    }
    _clock = event.OccurTime;
    _logger.Transfer("Processing:{}", event);
    double interval = _clock - _oldclock;
    _oldclock = _clock;
    if (_sysClients > 0)
    {
        _busyTime += interval;
        _areaN += _sysClients * interval;
        _areaS += (_sysClients - 1) * interval;
    }
    switch (event.Type)
    {
    case EventType::ARRIVAL:
        _logger.Information("Arrival:{}", event);
        for (auto &h : _onArrival)
        {
            h(this, event);
        }
        ProcessArrival(event);
        break;
    case EventType::DEPARTURE:
        ProcessDeparture(event);
        for (auto &h : _onDeparture)
        {
            h(this, event);
        }
        _logger.Information("Departure:{}", event);
        break;
    case EventType::NO_EVENT:
        break;
    case EventType::END:
        _logger.Information("End:{}", event);
        ProcessEnd(event);
        break;
    case EventType::PROBE:
        _logger.Information("Probe:{}", event);
        ProcessProbe(event);
    }
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
    collector.Reset();
}

Station::Station(std::string name, int station, bool registerCollector)
    : BaseStation(name), _stationIndex(station), collector{name, registerCollector}
{

    collector.AddMeasure(Measure<double>{"completitions", "unit"});
    collector.AddMeasure(Measure<double>{"arrivals", "unit"});
    collector.AddMeasure(Measure<double>{"sysClients", "unit"});
    collector.AddMeasure(Measure<double>{"maxClients", "unit"});
    collector.AddMeasure(Accumulator<double>{"avgInterArrival", "ms"});
    collector.AddMeasure(Accumulator<double>{"avgServiceTime", "ms"});
    collector.AddMeasure(Accumulator<double>{"avgDelay", "ms"});
    collector.AddMeasure(Accumulator<double>{"avgWaiting", "ms"});
    collector.AddMeasure(Accumulator<double>{"utilization", ""});
    collector.AddMeasure(Accumulator<double>{"throughput", "job/ms"});
    collector.AddMeasure(Accumulator<double>{"inputRate", ""});
    collector.AddMeasure(Accumulator<double>{"arrivalRate", ""});
    collector.AddMeasure(Accumulator<double>{"serviceRate", ""});
    collector.AddMeasure(Accumulator<double>{"traffic", ""});
    collector.AddMeasure(Accumulator<double>{"meanCustomerInQueue", "unit"});
    collector.AddMeasure(Accumulator<double>{"meanCustomerInService", ""});
    collector.AddMeasure(Accumulator<double>{"meanCustomerInSystem", ""});
}

void Station::Initialize()
{
}

void Station::Update()
{
    if (_completions == 0 || _arrivals == 0 || _oldclock == 0 || _busyTime == 0 || _lastArrival == 0)
        return;
    collector.lastTimeStamp = _oldclock;
    collector[Completions]->Accumulate(_completions);
    collector[Arrivals]->Accumulate(_arrivals);
    collector[sysclients]->Accumulate(_sysClients);
    collector[maxclients]->Accumulate(_maxClients);
    collector[avgInterArrival]->Accumulate(_oldclock / _arrivals);       /* Average inter-arrival time */
    collector[avgServiceTime]->Accumulate(_busyTime / _completions);     /* Average service time */
    collector[avgDelay]->Accumulate(_areaS / _completions);              /* Average delay time */
    collector[avgWaiting]->Accumulate(_areaN / _completions);            /* Average wait time */
    collector[utilization]->Accumulate(_busyTime / _oldclock);           /* Utilization */
    collector[throughput]->Accumulate(_completions / _oldclock);         /* Throughput */
    collector[inputRate]->Accumulate(_arrivals / _oldclock);             /* Input rate */
    collector[arrivalRate]->Accumulate(_arrivals / _oldclock);           /* Arriva rate */
    collector[serviceRate]->Accumulate(_completions / _busyTime);        /* Service rate */
    collector[traffic]->Accumulate(_busyTime / _lastArrival);            /* Traffic intensity */
    collector[meanCustomerInQueue]->Accumulate(_areaS / _oldclock);      /* Mean number of customers in queue */
    collector[meanCustomerInService]->Accumulate(_busyTime / _oldclock); /* Mean number of customers in service */
    collector[meanCustomerInSystem]->Accumulate(_areaN / _oldclock);     /* Mean number of customers in system */
    collector._samples++;
}
