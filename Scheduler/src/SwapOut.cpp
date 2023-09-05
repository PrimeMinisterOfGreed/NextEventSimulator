//
// Created by drfaust on 23/01/23.
//
#include "SwapOut.hpp"
#include "Options.hpp"
#include "Enums.hpp"
#include "rngs.hpp"
#include "rvgs.h"

void SwapOut::ProcessArrival(Event&evt)
{
    Station::ProcessArrival(evt);
    evt.OccurTime = _clock;
    evt.ServiceTime = 0;
    evt.Type = EventType::DEPARTURE;
    _scheduler->Schedule(evt);
}

void SwapOut::ProcessDeparture(Event&evt)
{
    Station::ProcessDeparture(evt);
    if ((*_swap)() > 0.4)
    {
        evt.Type = EventType::ARRIVAL;
        evt.OccurTime = _clock;
        evt.Station = Stations::RESERVE_STATION;
        _scheduler->Schedule(evt);
    }
}

SwapOut::SwapOut(ILogEngine *logger, IScheduler *scheduler) : Station(logger,Stations::SWAP_OUT)
{
    _scheduler = scheduler;
_swap = RandomStream::Global().GetStream([](auto& rng){
    return Uniform(0.0, 1.0);
});
    _name = "SWAPOUT";
}


