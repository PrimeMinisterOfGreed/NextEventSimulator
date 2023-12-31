#include "ReserveStation.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "Scheduler.hpp"
#include "SystemParameters.hpp"
#include "rvgs.h"

void ReserveStation::ProcessDeparture(Event &evt)
{

    FCFSStation::ProcessDeparture(evt);
    if (_counter <= SystemParameters::Parameters().multiProgrammingDegree)
    {
        evt.Type = ARRIVAL;
        evt.OccurTime = _clock;
        evt.ServiceTime = 0;
        evt.Station = SWAP_IN;
        _scheduler->Schedule(evt);
        _counter++;
    }
    else
    {
        _eventQueue.Enqueue(evt);
    }
}

ReserveStation::ReserveStation(IScheduler *scheduler)
    : FCFSStation(scheduler, "RESERVE_STATION", Stations::RESERVE_STATION),
      _swapinTime(9, [](auto rng) { return Exponential(210); })
{
    static_cast<Scheduler *>(scheduler)->GetStation(Stations::SWAP_OUT).value()->OnDeparture([this](auto s, auto evt) {
        _counter--;
    });
}

void ReserveStation::ProcessArrival(Event &evt)
{
    evt.ServiceTime = _swapinTime();
    FCFSStation::ProcessArrival(evt);
}
