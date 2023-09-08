#include "OperativeSystem.hpp"
#include "CPU.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Options.hpp"
#include "rngs.hpp"
#include "rvgs.h"

void OS::Execute()
{
    Initialize();
    while (!_end)
    {
        auto nextEvt = _eventQueue.Dequeue();
        ISimulator::_clock = nextEvt.OccurTime;
        OnEventProcess.Invoke(_stations);
        if (nextEvt.Station == Stations::RESERVE_STATION && nextEvt.Type == EventType::ARRIVAL)
        {
            double nextArrival = (*_nextArrival)() + ISimulator::_clock;
            auto evt =
                Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION), EventType::ARRIVAL,
                      Station::_clock, nextArrival, 0, nextArrival, Stations::RESERVE_STATION);
            Process(evt);
            Schedule(evt);
        }
        else if (nextEvt.Type == EventType::END)
            _end = true;
        else if (nextEvt.Station == Stations::SWAP_OUT && nextEvt.Type == EventType::DEPARTURE)
        {
            Process(nextEvt);
            OnProcessFinished.Invoke(_stations);
        }
        RouteToStation(nextEvt);
    }
}

OS::OS(double cpuTimeSlice, int multiProgrammingDegree)
    : Station("OS", 0),

      _cpu{Cpu(this, cpuTimeSlice)}, _reserveStation{ReserveStation(multiProgrammingDegree, this)},
      _io1{IOStation(this, Stations::IO_1)}, _io2{IOStation(this, Stations::IO_2)}, _swapOut{SwapOut(this)},
      _swapIn{SwapIn(this)}
{
    _nextArrival = RandomStream::Global().GetStream([](auto &rng) { return Exponential(0.2); });
    _stations = std::vector<Station *>({this, &_cpu, &_reserveStation, &_io1, &_io2, &_swapIn, &_swapOut});
}

void OS::Reset()
{
    Station::Reset();
    _end = false;
    for (auto station : _stations)
    {
        station->Reset();
    }
    _eventQueue.Clear();
}

void OS::RouteToStation(Event &evt)
{
    if (evt.Station == -1)
    {
        Process(evt);
        return;
    }
    for (auto station : _stations)
    {
        if (evt.Station == station->stationIndex())
        {
            station->Process(evt);
            return;
        }
    }
    throw std::invalid_argument("Cannot find a suitable station for the given argument");
}

void OS::Initialize()
{

    Schedule(Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION), EventType::ARRIVAL,
                   Station::_clock, 0, 0, 0, Stations::RESERVE_STATION));
}

void OS::Schedule(Event event)
{
    _eventQueue.Insert(event, [](const Event &a, const Event &b) { return a.OccurTime > b.OccurTime; });
}
