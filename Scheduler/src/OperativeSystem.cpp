#include "OperativeSystem.hpp"
#include "CPU.hpp"
#include "DataWriter.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Options.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"

void OS::Execute()
{
    Initialize();
    while (!_end)
    {
        auto nextEvt = _eventQueue.Dequeue();
        _clock = nextEvt.OccurTime;
        if (nextEvt.Station == Stations::RESERVE_STATION && nextEvt.Type == EventType::ARRIVAL)
        {
            OnEventProcess(_stations);
            double nextArrival = (*_nextArrival)() + _clock;
            auto evt = Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION),
                             EventType::ARRIVAL, _clock, nextArrival, 0, nextArrival, Stations::RESERVE_STATION);
            Process(evt);
            Schedule(evt);
        }
        else if (nextEvt.Type == EventType::END)
            _end = true;
        else if (nextEvt.Station == Stations::SWAP_OUT && nextEvt.Type == EventType::DEPARTURE)
        {
            Process(nextEvt);
            OnProcessFinished(_stations);
        }
        RouteToStation(nextEvt);
    }
}

OS::OS()
    : Station("OS", 0),

      _cpu{Cpu(this)}, _reserveStation{ReserveStation(this)}, _io1{IOStation(this, Stations::IO_1)},
      _io2{IOStation(this, Stations::IO_2)}, _swapOut{SwapOut(this)}, _swapIn{SwapIn(this)}
{
    DataWriter::Instance().header = collector.Header();
    _nextArrival = RandomStream::Global().GetStream(
        [](auto &rng) { return Exponential(SystemParameters::Parameters().workStationThinkTime); });
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
    if (evt.Station == 0)
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

void OS::ProcessProbe(Event &evt)
{
    Station::ProcessProbe(evt);
    for (auto station : _stations)
    {
        if (station->stationIndex() != this->stationIndex())
            station->Process(evt);
    }
}

void OS::Schedule(Event event)
{
    _logger->TraceTransfer("Scheduling event for station {} at time {}", event.Station, event.OccurTime);
    _eventQueue.Insert(event, [](const Event &a, const Event &b) { return a.OccurTime > b.OccurTime; });
}
