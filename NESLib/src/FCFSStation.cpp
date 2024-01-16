#include "FCFSStation.hpp"
#include "Core.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Station.hpp"
#include <fmt/core.h>

void FCFSStation::ProcessArrival(Event &evt)
{
    if (evt.SubType != MAINTENANCE)
        Station::ProcessArrival(evt);
    if (!_eventUnderProcess.has_value())
    {
        evt.CreateTime = _clock;
        evt.ArrivalTime = _clock;
        evt.OccurTime = _clock + evt.ServiceTime;
        evt.Type = EventType::DEPARTURE;
        _eventUnderProcess.emplace(evt);
        _scheduler->Schedule(evt);
    }
    else
    {
        _eventList.Enqueue(evt);
    }
}

void FCFSStation::ProcessDeparture(Event &evt)
{

    core_assert(evt == _eventUnderProcess.value(),"event {} is not equal to event under process {}", evt.Name, _eventUnderProcess->Name);
    if (_eventUnderProcess->SubType != MAINTENANCE)
        Station::ProcessDeparture(_eventUnderProcess.value());
    if (_sysClients > 0)
    {
        _eventUnderProcess.emplace(_eventList.Dequeue());
        _eventUnderProcess->ArrivalTime = _clock;
        _eventUnderProcess->CreateTime = _clock;
        _eventUnderProcess->OccurTime = _clock + _eventUnderProcess->ServiceTime;
        _eventUnderProcess->Type = EventType::DEPARTURE;
        _scheduler->Schedule(_eventUnderProcess.value());
    }
    else
        _eventUnderProcess.reset();
}

void FCFSStation::ProcessEnd(Event &evt)
{
    Station::ProcessEnd(evt);
}

void FCFSStation::ProcessProbe(Event &evt)
{
    Station::ProcessProbe(evt);
}

void FCFSStation::Reset()
{
    Station::Reset();
}

FCFSStation::FCFSStation(IScheduler *scheduler, std::string name, int stationIndex)
    : Station(name, stationIndex), _scheduler(scheduler)
{
}
