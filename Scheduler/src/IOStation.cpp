#include "IOStation.hpp"
#include "DataCollector.hpp"
#include "Enums.hpp"
#include "FCFSStation.hpp"
#include "Options.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <regex>

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

IOStation::IOStation(IScheduler *scheduler, int stationIndex)
    : FCFSStation(scheduler, stationIndex == Stations::IO_1 ? "IO1" : "IO2", stationIndex)
{
    _serviceTime = RandomStream::Global().GetStream([this](auto &rng) {
        return Exponential(_stationIndex == Stations::IO_1 ? SystemParameters::Parameters().averageIO1
                                                           : SystemParameters::Parameters().averageIO2);
    });
    _name = _stationIndex == 4 ? "IO1" : "IO2";
}
