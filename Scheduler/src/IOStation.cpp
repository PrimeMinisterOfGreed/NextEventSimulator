#include "IOStation.hpp"
#include "Enums.hpp"
#include "FCFSStation.hpp"
#include "Options.hpp"
#include "rngs.hpp"
#include "rvgs.h"

void IOStation::ProcessArrival(Event &evt)
{
    evt.ServiceTime = (*_serviceTime)();
    FCFSStation::ProcessArrival(evt);
}

void IOStation::ProcessDeparture(Event &evt)
{
    FCFSStation::ProcessDeparture(evt);
    evt.Station = Stations::CPU;
    evt.OccurTime = _clock;
    evt.Type = EventType::ARRIVAL;
    _scheduler->Schedule(evt);
}

IOStation::IOStation(IScheduler *scheduler, int stationIndex) : FCFSStation(scheduler, "", stationIndex)
{
    _serviceTime = RandomStream::Global().GetStream(
        [this](auto &rng) { return Exponential(_stationIndex == 4 ? 25 : 5.555555556); });
    _name = _stationIndex == 4 ? "IO1" : "IO2";
}
