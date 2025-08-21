//
// Created by drfaust on 23/01/23.
//
#include "SwapIn.hpp"
#include "Enums.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"

SwapIn::SwapIn(IScheduler *scheduler)
    : FCFSStation(scheduler, "SWAP_IN", Stations::SWAP_IN),
      _serviceTime(
          VariableStream(6, [](auto &rng) { return Exponential(SystemParameters::Parameters().averageSwapIn); }))
{
}

void SwapIn::ProcessArrival(Event &evt)
{
    evt.ServiceTime = _serviceTime();
    FCFSStation::ProcessArrival(evt);
}

void SwapIn::ProcessDeparture(Event &evt)
{
    FCFSStation::ProcessDeparture(evt);
    evt.OccurTime = _clock;
    evt.Station = Stations::CPU;
    evt.ArrivalTime = _clock;
    evt.Type = EventType::ARRIVAL;
    _scheduler->Schedule(evt);
}
