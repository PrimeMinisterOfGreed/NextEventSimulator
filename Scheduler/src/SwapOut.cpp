//
// Created by drfaust on 23/01/23.
//
#include "SwapOut.hpp"
#include "Options.hpp"
#include "Enums.hpp"

void SwapOut::ProcessArrival(Event *evt)
{
    Station::ProcessArrival(evt);
    evt->OccurTime = _clock;
    evt->ServiceTime = 0;
    evt->Type = EventType::DEPARTURE;
    _scheduler->Schedule(evt);
}

void SwapOut::ProcessDeparture(Event *evt)
{
    Station::ProcessDeparture(evt);
    if (_swap() <= 0.4)
    {
        delete evt;
    }
    else
    {
        evt->Type = EventType::ARRIVAL;
        evt->OccurTime = _clock;
        evt->Station = Stations::RESERVE_STATION;
        _scheduler->Schedule(evt);
    }
}

SwapOut::SwapOut(ILogEngine *logger, IScheduler *scheduler) : Station(logger,Stations::SWAP_OUT),
                                                              _swap{*new RandomVariable(streamGenerator)}
{
    _scheduler = scheduler;

    _name = "SWAPOUT";
}


