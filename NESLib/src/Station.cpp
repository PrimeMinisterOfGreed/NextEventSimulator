#include "Station.hpp"
#include "DataCollector.hpp"
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
    _arrivals++;
    _sysClients++;
    _lastArrival = evt.ArrivalTime;
    if (_sysClients > _maxClients)
        _maxClients = _sysClients;
}

void Station::ProcessDeparture(Event &evt)
{
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
}

void Station::Process(Event &event)
{
    if (event.Type == END)
    {
        ProcessEnd(event);
        return;
    }
    _clock = event.OccurTime;
    _logger.Transfer("Processing:{}", event);
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
        _logger.Information("Arrival:{}", event);
        break;
    case EventType::DEPARTURE:
        ProcessDeparture(event);
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
}

Station::Station(std::string name, int station) : _name(name), _stationIndex(station), collector{name}, _logger{name}
{
    collector.AddMeasure(sptr<Measure<double>>(new Measure<double>{"completitions", "unit"}));
    collector.AddMeasure(sptr<Measure<double>>(new Measure<double>{"arrivals", "unit"}));
    collector.AddMeasure(sptr<Measure<double>>(new Measure<double>{"sysClients", "unit"}));
    collector.AddMeasure(sptr<Measure<double>>(new Measure<double>{"maxClients", "unit"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"avgInterArrival", "ms"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"avgServiceTime", "ms"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"avgDelay", "ms"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"avgWaiting", "ms"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"utilization", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"throughput", "job/ms"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"inputRate", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"arrivalRate", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"serviceRate", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"traffic", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"meanCustomerInQueue", "unit"}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"meanCustomerInService", ""}));
    collector.AddMeasure(sptr<Measure<double>>(new Accumulator<double>{"meanCustomerInSystem", ""}));
}

void Station::Initialize()
{
}

void Station::Update()
{
    collector.lastTimeStamp = _oldclock;
    collector[Completions]->Accumulate(_completions);
    collector[Arrivals]->Accumulate(_arrivals);
    collector[sysclients]->Accumulate(_sysClients);
    collector[maxclients]->Accumulate(_maxClients);
    collector[avgInterArrival]->Accumulate(_oldclock / _arrivals);           /* Average inter-arrival time */
    collector[avgServiceTime]->Accumulate(_busyTime / _completions);         /* Average service time */
    collector[avgDelay]->Accumulate(_areaS / _completions);                  /* Average delay time */
    collector[avgWaiting]->Accumulate(_areaN / _completions);                /* Average wait time */
    collector[utilization]->Accumulate(_busyTime / _observationPeriod);      /* Utilization */
    collector[throughput]->Accumulate(_completions / _observationPeriod);    /* Throughput */
    collector[inputRate]->Accumulate(_arrivals / _oldclock);                 /* Input rate */
    collector[arrivalRate]->Accumulate(_arrivals / _observationPeriod);      /* Arriva rate */
    collector[serviceRate]->Accumulate(_completions / _busyTime);            /* Service rate */
    collector[traffic]->Accumulate(_busyTime / _lastArrival);                /* Traffic intensity */
    collector[meanCustomerInQueue]->Accumulate(_areaS / _observationPeriod); /* Mean number of customers in queue */
    collector[meanCustomerInService]->Accumulate(_busyTime /
                                                 _observationPeriod);         /* Mean number of customers in service */
    collector[meanCustomerInSystem]->Accumulate(_areaS / _observationPeriod); /* Mean number of customers in system */
    collector._samples++;
}

DataCollector Station::Data()
{
    return collector;
}
