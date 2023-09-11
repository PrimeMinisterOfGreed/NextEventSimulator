#include "ReserveStation.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "SystemParameters.hpp"

void ReserveStation::ProcessDeparture(Event &evt)
{
    auto sysActive = dynamic_cast<Station *>(_scheduler)->sysClients();
    FCFSStation::ProcessDeparture(evt);
    if (sysActive <= SystemParameters::Parameters().multiProgrammingDegree)
    {
        evt.Type = ARRIVAL;
        evt.OccurTime = _clock;
        evt.ServiceTime = 0;
        evt.Station = SWAP_IN;
        _scheduler->Schedule(evt);
    }
    else
    {
        _eventQueue.Enqueue(evt);
    }
}

ReserveStation::ReserveStation(IScheduler *scheduler)
    : FCFSStation(scheduler, "RESERVE_STATION", Stations::RESERVE_STATION)
{
}

void ReserveStation::ProcessArrival(Event &evt)
{
    FCFSStation::ProcessArrival(evt);
}
