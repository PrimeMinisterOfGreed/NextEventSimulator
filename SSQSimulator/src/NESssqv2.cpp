#include "NESssqv2.hpp"
#include "LogEngine.hpp"
#include "Options.hpp"
#include <cstdint>
#include <ostream>
#include <string>


void NESssq::Arrival(Event &e)
{
    _statistic.nsys++;
    systemClients(_statistic.nsys);
    _statistic.narr++;
    if (_statistic.nsys > _statistic.nsysMax)
        _statistic.nsysMax = _statistic.nsys;
    e.CreateTime = _clock.clock;
    e.ArrivalTime = _clock.clock;
    if (_statistic.nsys == 1)
    {
        e.Type = EventType::DEPARTURE;
        e.OccurTime = _clock.clock + e.ServiceTime;
        Schedule(e);
    }
    else
    {
        e.Type = EventType::NO_EVENT;
        e.OccurTime = 0.0;
        _inputQueue.Enqueue(new Event(e));
        _statistic.ndelayed++;
    }
    if (!_provider->end())
    {
        auto arrival = _provider->GetArrival();
        auto service = _provider->GetService();
        _statistic.Last_arrival = arrival;
        Event &newEvent = *new Event(std::string("J") + std::to_string(_statistic.narr), EventType::ARRIVAL,
                                     _clock.clock, arrival, service, arrival);
        Schedule(newEvent);
    }
}

void NESssq::Schedule(Event &e)
{
    _futureEvents.Insert(&e, [](const Event &a, const Event &b) { return a.OccurTime >= b.OccurTime; });
}

void NESssq::Departure(Event &e)
{
    _statistic.nsys--;
    _statistic.ncom++;
    systemClients(_statistic.nsys);
    if (_statistic.nsys > 0)
    {
        Event &newE = _inputQueue.Dequeue();
        newE.Type = EventType::DEPARTURE;
        newE.OccurTime = _clock.clock + newE.ServiceTime;
        Schedule(newE);
    }
    delete &e;
    _statistic.return_number++;
}

void NESssq::End(Event &e)
{
    _stop = true;
    std::cout << "Last Time= " << std::to_string(_clock.lasttime) << ", Old Clock: " << std::to_string(_clock.oldclock)
              << std::endl;
    _clock.Stop = _clock.lasttime;
    delete &e;
    _statistic.return_number++;
}

void NESssq::Probe(Event &e)
{
    _statistic.clientTarget = _statistic.nsys;
}

void NESssq::Initialize()
{
    _logger->TraceInformation("Initializing");
    _clock.End_time = endTime;
    _statistic.Target_time = targetTime;
    _clock.Arrival = _provider->GetArrival();
    _clock.Service = _provider->GetService();
    if (!_provider->end())
    {
        Schedule(*new Event(std::string("J") + std::to_string(jobIndex), EventType::ARRIVAL, _clock.clock,
                            _clock.Arrival, _clock.Service, _clock.Arrival));

        if (optionsMap.count("-probe"))
        {
            auto probes = optionsMap.at("-probe").as<std::vector<int>>();
            for (auto &probe : probes)
            {
                Schedule(*new Event(std::string("PROBE"), EventType::PROBE, _clock.clock, probe, 0, 0));
            }
        }
        else
            Schedule(*new Event(std::string("PROBE"), EventType::PROBE, _clock.clock, targetTime, 0, 0));

        Schedule(*new Event(std::string("END"), EventType::END, _clock.clock, endTime, 0, 0));
    }
    std::cout << std::string("Initial Future Event List: ") << _futureEvents.ToString() << std::endl;
    std::cout << std::string("Initial Server queue: ") << _inputQueue.ToString() << std::endl;
}

void NESssq::Execute()
{
    Initialize();
    while (!_stop)
    {
        Engine();
    }
}

void NESssq::Engine()
{
    auto &newEvent = _futureEvents.Pull();
    _clock.oldclock = _clock.clock;
    _clock.clock = newEvent.OccurTime;
    double interval = _clock.clock - _clock.oldclock;
    if (_statistic.nsys > 0)
    {
        _clock.Busy = _clock.Busy + interval;
        _statistic.Area_n += _statistic.nsys * interval;
        _statistic.Area_d += (_statistic.nsys - 1) * interval;
    }
    switch (newEvent.Type)
    {
    case EventType::ARRIVAL:
        Arrival(newEvent);
        break;

    case EventType::DEPARTURE:
        Departure(newEvent);
        break;

    case EventType::PROBE:
        Probe(newEvent);
        break;

    case EventType::END:
        End(newEvent);
        break;
    default:
        break;
    }
    _statistic.event_counter++;
    _clock.lasttime = _clock.clock;
}

void NESssq::Report()
{
    ISSQSimulator::Report();

    _clock.ObservPeriod = _clock.Stop - _clock.Start;

    // ************************ EXERCISE 1 **********************

    
    _statistic.E_of_sigma = _statistic.Last_arrival / _statistic.narr; /* Average inter-arrival time */
    _statistic.E_of_S = _clock.Busy / _statistic.ncom;                 /* Average service time */
    _statistic.E_of_D = _statistic.Area_d / _statistic.ncom;           /* Average delay time */
    _statistic.E_of_W = _statistic.Area_n / _statistic.ncom;

    _statistic.U = _clock.Busy / _clock.ObservPeriod;             /* Utilization */
    _statistic.X = _statistic.ncom / _clock.ObservPeriod;         /* Throughput */
    _statistic.Theta = _statistic.narr / _statistic.Last_arrival; /* Input rate */
    _statistic.Lambda = _statistic.narr / _clock.ObservPeriod;    /* Arriva rate */
    _statistic.Mu = _statistic.ncom / _clock.Busy;                /* Service rate */
    _statistic.Rho = _clock.Busy / _statistic.Last_arrival;       /* Traffic intensity */
    _statistic.E_of_q = _statistic.Area_d / _clock.ObservPeriod;  /* Mean number of customers in queue */
    _statistic.E_of_y = _clock.Busy / _clock.ObservPeriod;        /* Mean number of customers in service */
    _statistic.E_of_n = _statistic.Area_n / _clock.ObservPeriod;  /* Mean number of customers at server */
    _logger->TraceResult("{0}Statistics\n{1}", PrintDivisor<'#'>(), _statistic.ToString());
    _logger->TraceResult("{}Clock\n{}", PrintDivisor<'#'>(), _clock.ToString());
    _logger->TraceResult("{}Final Event List\n{}", PrintDivisor<'#'>(), _futureEvents.ToString());
}

const std::string &EventList::ToString()
{
    std::stringstream buffer{};
    auto ptr = begin();
    while (ptr != end() && &ptr() != nullptr)
    {
        buffer << "|Job:" << (*ptr).Name << ",TA:" << std::to_string((*ptr).ArrivalTime)
               << " ,TS:" << std::to_string((*ptr).ServiceTime) << ", Type:" << (char)(*ptr).Type
               << ", TO:" << std::to_string((*ptr).OccurTime) << "|-->";
        ptr++;
    }
    return *new std::string(buffer.str());
}

std::string &Statistics::ToString()
{
    std::stringstream buffer{};
    buffer << makeformat("\nNumber of processed events            = {}", event_counter)
        << makeformat("\nMaximun number of customers in system = {}", nsysMax)
        << makeformat("\nNumber of new generated nodes         = {}", node_number)
        << makeformat("\nNumber of used nodes                  = {}", return_number)
        << makeformat("\nNumber of arrivals    = {}<< ......... Number of completions = {}", narr, ncom)
        << makeformat("\nNumber of delayed customers           = {}", ndelayed)
        << makeformat("\n   Average inter_arrival time ...................... = {}", E_of_sigma)
        << makeformat("\n   Average service time ............................ = {}", E_of_S)
        << makeformat("\n   Average delay time .............................. = {}", E_of_D)
        << makeformat("\n   Average Waiting Time in the Server .............. = {}", E_of_W) << makeformat("\n")
        << makeformat("\n   Input rate ...................................... = {}", Theta)
        << makeformat("\n   Service rate .................................... = {}", Mu)
        << makeformat("\n   Traffic intensity ............................... = {}", Rho)
        << makeformat("\n   Throughput ...................................... = {}", X)
        << makeformat("\n   Server Utilization .............................. = {}", U)
        << makeformat("\n   Average number at server ........................ = {}", E_of_n)
        << makeformat("\n   Average number in queue ......................... = {}", E_of_q)
        << makeformat("\n   Average number in service ....................... = {}", E_of_y)
        << makeformat("\n   Number of customers in the system at time %f .... = {}", Target_time, clientTarget);
    return *new std::string(buffer.str());
}

std::string &Clock::ToString()
{
    
    return *new std::string(makeformat("\nStart Simulation Time = {}; ... Last Simulation Time  = {}", Start, Stop)+
    makeformat("\nLength of Observation Period          = {}", ObservPeriod));

    
}
