#include "Station.hpp"
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
        break;
    case EventType::NO_EVENT:
    case EventType::END:
    case EventType::PROBE:
        break;
    }
}

std::string &Station::ToString()
{
    std::stringstream buffer{};
    buffer << makeformat("Statistics of station:{}\n", _name) << makeformat("Arrivals:{}\n", _arrivals)
           << makeformat("Clock:{}\n", _clock) << makeformat("Last Arrival:{}\n", _lastArrival)
           << makeformat("Completions:{}\n", _completions)
        << GetStatistics().ToString();
    return *new std::string(buffer.str());
}

StationStatistic Station::GetStatistics()
{
    StationStatistic result{};
    result.E_of_sigma = _oldclock / _arrivals;      /* Average inter-arrival time */
    result.E_of_S = _busyTime / _completions;       /* Average service time */
    result.E_of_D = _areaS / _completions;          /* Average delay time */
    result.E_of_W = _areaN / _completions;          /* Average wait time */
    result.U = _busyTime / _observationPeriod;      /* Utilization */
    result.X = _completions / _observationPeriod;   /* Throughput */
    result.Theta = _arrivals / _oldclock;           /* Input rate */
    result.Lambda = _arrivals / _observationPeriod; /* Arriva rate */
    result.Mu = _completions / _busyTime;           /* Service rate */
    result.Rho = _busyTime / _lastArrival;          /* Traffic intensity */
    result.E_of_q = _areaS / _observationPeriod;    /* Mean number of customers in queue */
    result.E_of_y = _busyTime / _observationPeriod; /* Mean number of customers in service */
    result.E_of_n = _areaS / _observationPeriod;    /* Mean number of customers in system */
    return result;
}

Station::Station(ILogEngine *logger) : _logger(logger)
{
}

std::string &StationStatistic::ToString() const
{
    std::stringstream buf{};
    buf << makeformat("\n   Average inter_arrival time ...................... = {}", E_of_sigma)
        << makeformat("\n   Average service time ............................ = {}", E_of_S)
        << makeformat("\n   Average delay time .............................. = {}", E_of_D)
        << makeformat("\n   Average Waiting Time in the Server .............. = {}", E_of_W)
        << makeformat("\n   Input rate ...................................... = {}", Theta)
        << makeformat("\n   Service rate .................................... = {}", Mu)
        << makeformat("\n   Traffic intensity ............................... = {}", Rho)
        << makeformat("\n   Throughput ...................................... = {}", X)
        << makeformat("\n   Server Utilization .............................. = {}", U)
        << makeformat("\n   Average number at server ........................ = {}", E_of_n)
        << makeformat("\n   Average number in queue ......................... = {}", E_of_q)
        << makeformat("\n   Average number in service ....................... = {}", E_of_y);
    return *new std::string(buf.str());
}
