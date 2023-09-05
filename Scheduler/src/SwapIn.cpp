//
// Created by drfaust on 23/01/23.
//
#include "SwapIn.hpp"
#include "Options.hpp"
#include "Enums.hpp"
#include "rngs.hpp"
#include "rvgs.h"

SwapIn::SwapIn(ILogEngine* logger, IScheduler* scheduler) : FCFSStation(logger, scheduler,
	Stations::SWAP_IN)
{
	_serviceTime = RandomStream::Global().GetStream(
		[](auto&rng){
			return Exponential(4.761904762);
		}
	);
	_name = "SWAPIN";
}

void SwapIn::ProcessArrival(Event* evt)
{
	evt->ServiceTime = (*_serviceTime)();
	FCFSStation::ProcessArrival(evt);
}

void SwapIn::ProcessDeparture(Event* evt)
{
	FCFSStation::ProcessDeparture(evt);
	evt->OccurTime = _clock;
	evt->Station = Stations::CPU;
	evt->ArrivalTime = _clock;
	evt->Type = EventType::ARRIVAL;
	_scheduler->Schedule(evt);
}


