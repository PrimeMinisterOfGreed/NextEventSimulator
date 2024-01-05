#include "ReserveStation.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "rvgs.h"

void ReserveStation::ProcessDeparture(Event &evt)
{
    panic("This station has not departure options");
}

ReserveStation::ReserveStation(IScheduler *scheduler)
    : FCFSStation(scheduler, "RESERVE_STATION", Stations::RESERVE_STATION)
{
    static_cast<Scheduler *>(scheduler)->GetStation(Stations::SWAP_OUT).value()->OnDeparture([this](auto s, auto evt) {
        _counter--;
    });
}

void ReserveStation::ProcessArrival(Event &evt)
{
    evt.ServiceTime = 0;
    Station::ProcessArrival(evt);
    if (_counter <= SystemParameters::Parameters().multiProgrammingDegree)
    {
        Event &tgt = evt;
        Station::ProcessDeparture(evt);
        if (_eventList.Count() > 0)
        {
            tgt = _eventList.Dequeue();
            _eventList.Enqueue(evt);
        }
        tgt.Type = ARRIVAL;
        tgt.OccurTime = _clock;
        tgt.Station = SWAP_IN;
        _scheduler->Schedule(tgt);
        _counter++;
    }
    else
    {
        _eventList.Enqueue(evt);
    }
}
