//
// Created by drfaust on 23/01/23.
//
#include "SwapOut.hpp"
#include "Enums.hpp"
#include "rngs.hpp"

void SwapOut::ProcessArrival(Event &evt)
{
    Station::ProcessArrival(evt);
    evt.OccurTime = _clock;
    evt.ServiceTime = 0;
    evt.Type = EventType::DEPARTURE;
    _scheduler->Schedule(evt);
}

void SwapOut::ProcessDeparture(Event &evt)
{
    static CompositionStream router(
        6, {0.6, 0.4}, [](auto rng) { return 0; }, [](auto rng) { return RESERVE_STATION; });
    Station::ProcessDeparture(evt);
    evt.Type = EventType::ARRIVAL;
    evt.OccurTime = _clock;
    evt.Station = router();
    _logger.Information("Swapping out Process:{} to {}", evt,
                        evt.Station == RESERVE_STATION ? "Reserve" : "DelayStation");
    _scheduler->Schedule(evt);
}

SwapOut::SwapOut(IScheduler *scheduler) : Station("SWAP_OUT", Stations::SWAP_OUT)
{
    _scheduler = scheduler;
}
