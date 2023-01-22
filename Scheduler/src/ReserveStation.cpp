#include "ReserveStation.hpp"
#include "FCFSStation.hpp"
#include "Enums.hpp"

void ReserveStation::ProcessArrival(Event *evt)
{
    auto sysActive = dynamic_cast<ISimulator *>(_scheduler)->clientsInSystem();
    if (sysActive > _multiProgrammingDegree)
    {
        //reject or accept the new process
    }
}

void ReserveStation::ProcessDeparture(Event *evt)
{
    FCFSStation::ProcessDeparture(evt);

    evt->Type = EventType::ARRIVAL;
    evt->OccurTime = _clock;
    evt->ArrivalTime = _clock;
    evt->Station = Stations::SWAP_IN;
    _scheduler->Schedule(evt);
}

