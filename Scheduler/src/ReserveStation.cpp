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
        if (_eventQueue.Count() == 0)
        {
            evt.Type = ARRIVAL;
            evt.OccurTime = _clock;
            evt.ServiceTime = 0;
            evt.Station = SWAP_IN;
            _scheduler->Schedule(evt);
        }
        else
        {
            auto nextEvt = _eventQueue.Dequeue();
            nextEvt.Type = ARRIVAL;
            nextEvt.OccurTime = _clock;
            nextEvt.ServiceTime = 0;
            nextEvt.Station = SWAP_IN;
            _scheduler->Schedule(nextEvt);
            _eventQueue.Enqueue(evt);
        }
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
