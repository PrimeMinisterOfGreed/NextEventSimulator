#include "ReserveStation.hpp"
#include "Enums.hpp"
#include "FCFSStation.hpp"

void ReserveStation::ProcessDeparture(Event &evt)
{
    auto sysActive = dynamic_cast<Station *>(_scheduler)->sysClients();
    FCFSStation::ProcessDeparture(evt);
    if (sysActive <= _multiProgrammingDegree)
    {
        evt.Type = ARRIVAL;
        evt.OccurTime = _clock;
        evt.ServiceTime = 0;
        evt.Station = SWAP_IN;
        _scheduler->Schedule(evt);
    }
}

ReserveStation::ReserveStation(int multiProgrammingDegree, IScheduler *scheduler)
    : _multiProgrammingDegree(multiProgrammingDegree),
      FCFSStation(scheduler, "RESERVE_STATION", Stations::RESERVE_STATION)
{
}

void ReserveStation::ProcessArrival(Event &evt)
{
    FCFSStation::ProcessArrival(evt);
}
