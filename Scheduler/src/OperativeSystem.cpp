#include "OperativeSystem.hpp"
#include "Event.hpp"
#include "Generators.hpp"
#include "Enums.hpp"
#include "Options.hpp"

void OS::Execute()
{
    Initialize();
    while (!_end)
    {
        auto nextEvt = &_eventQueue.Dequeue();
        ISimulator::_clock = nextEvt->OccurTime;
        if (nextEvt->Station == Stations::RESERVE_STATION && nextEvt->Type == EventType::ARRIVAL)
        {
            double nextArrival = _nextArrival->GetValue() + ISimulator::_clock;
            auto evt = new Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION),
                                 EventType::ARRIVAL,
                                 Station::_clock, nextArrival, 0, nextArrival, Stations::RESERVE_STATION);
            Schedule(evt);
            Process(evt);
        }
        else if (nextEvt->Type == EventType::END)
            _end = true;
        else if (nextEvt->Station == Stations::SWAP_OUT && nextEvt->Type == EventType::DEPARTURE)
        {
            Process(nextEvt);
        }
        RouteToStation(nextEvt);
    }
}

void OS::Report()
{
    _logger->TraceResult("Simulator Statistics:\n{}", GetStatistics().ToString());
}

OS::OS(ILogEngine *logger, double cpuTimeSlice, int multiProgrammingDegree) : Station(logger, -1)
{
    _cpu = new Cpu(logger, this, cpuTimeSlice);
    _reserveStation = new ReserveStation(multiProgrammingDegree, logger, this);
    _io1 = new IOStation(logger, this, Stations::IO_1);
    _io2 = new IOStation(logger, this, Stations::IO_2);
    _swapOut = new SwapOut(logger, this);
    _swapIn = new SwapIn(logger, this);
    _stations = new std::vector<Station *>({_cpu, _reserveStation, _io1, _io2, _swapIn, _swapOut});
    _nextArrival = new NegExpVariable(0.2, streamGenerator);
    _name = "OS";
}

void OS::Reset()
{
    _end = false;
}

void OS::RouteToStation(Event *evt)
{
    if (evt->Station == -1)
    {
        Process(evt);
        return;
    }
    for (auto station: *_stations)
    {
        if (evt->Station == station->stationIndex())
        {
            station->Process(evt);
            return;
        }
    }
    throw std::invalid_argument("Cannot find a suitable station for the given argument");
}

void OS::Initialize()
{

    Schedule(new Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION), EventType::ARRIVAL,
                       Station::_clock, 0, 0, 0, Stations::RESERVE_STATION));
    Schedule(new Event("END", END, ISimulator::_clock, 10000, 0, 0, -1));
}

void OS::Schedule(Event *event)
{
    _eventQueue.Insert(event, [](const Event &a, const Event &b)
    { return a.OccurTime > b.OccurTime; });
}
