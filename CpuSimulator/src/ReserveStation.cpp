/**
 * @file ReserveStation.cpp
 * @author matteo.ielacqua
 * @brief implementazione della stazione di riserva, generica istanza di una stazione FCFS
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ReserveStation.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "rvgs.h"

void ReserveStation::ProcessDeparture(Event &evt)
{
    panic("This station has not departure options");
}

/**
 * @brief costruttore per la stazione di riserva,
 * la stazione di riserva non ha eventi di partenza, poichè scoda eventi di arrivo 
 * per la CPU ogni volta che un cliente lascia la stazione di swap out
 * @param scheduler 
 */
ReserveStation::ReserveStation(IScheduler *scheduler)
    : FCFSStation(scheduler, "RESERVE_STATION", Stations::RESERVE_STATION)
{
    static_cast<Scheduler *>(scheduler)->GetStation(Stations::SWAP_OUT).value()->OnDeparture([this](auto s, auto evt) {
        _counter--;
        if (_eventList.Count() > 0 && _counter < SystemParameters::Parameters().multiProgrammingDegree)
        {
            auto ev = _eventList.Dequeue();
            ev.Station = SWAP_IN;
            ev.Type = ARRIVAL;
            ev.OccurTime = evt.OccurTime;
            _scheduler->Schedule(ev);
            _counter++;
            Station::ProcessDeparture(ev);
        }
    });
}


void ReserveStation::ProcessArrival(Event &evt)
{
    evt.ServiceTime = 0;
    Station::ProcessArrival(evt);
    if (_counter < SystemParameters::Parameters().multiProgrammingDegree)
    {
        _counter++;
        evt.OccurTime = clock();
        evt.Station = SWAP_IN;
        _scheduler->Schedule(evt);
        Station::ProcessDeparture(evt);
    }
    else
    {
        _eventList.Enqueue(evt);
    }
}
