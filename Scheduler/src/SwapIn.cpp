//
// Created by drfaust on 23/01/23.
//
#include "SwapIn.hpp"
#include "Options.hpp"
#include "Enums.hpp"

SwapIn::SwapIn(ILogEngine *logger, IScheduler *scheduler, int stationIndex) : FCFSStation(logger, scheduler,
                                                                                          stationIndex)
{
    _serviceTime = new NegExpVariable(4.761904762, streamGenerator.get());
}

void SwapIn::ProcessArrival(Event *evt)
{
    evt->ServiceTime = _serviceTime->GetValue();
    FCFSStation::ProcessArrival(evt);
}

void SwapIn::ProcessDeparture(Event *evt)
{
    FCFSStation::ProcessDeparture(evt);
    evt->OccurTime = _clock;
    evt->Station = Stations::CPU;
    evt->ArrivalTime = _clock;
    evt->Type = EventType::ARRIVAL;
    _scheduler->Schedule(evt);
}


