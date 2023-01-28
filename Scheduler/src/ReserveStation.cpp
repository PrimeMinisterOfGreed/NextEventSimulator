#include "ReserveStation.hpp"
#include "FCFSStation.hpp"
#include "Enums.hpp"


void ReserveStation::ProcessDeparture(Event *evt)
{
    auto sysActive = dynamic_cast<Station *>(_scheduler)->sysClients();
    FCFSStation::ProcessDeparture(evt);
    if (sysActive > _multiProgrammingDegree)
    {
        delete evt; //end of the line
    }
    else
    {
        evt->Type = ARRIVAL;
        evt->OccurTime = _clock;
        evt->ServiceTime = 0;
        evt->Station= SWAP_IN;
        _scheduler->Schedule(evt);
    }
}

ReserveStation::ReserveStation(int multiProgrammingDegree, ILogEngine *logger, IScheduler *scheduler)
        : _multiProgrammingDegree(multiProgrammingDegree),
          FCFSStation(logger, scheduler, Stations::RESERVE_STATION)
{
    _name = "RESERVE_STATION";
}

void ReserveStation::ProcessArrival(Event *evt)
{
    FCFSStation::ProcessArrival(evt);
}

