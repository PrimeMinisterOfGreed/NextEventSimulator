#include "IOStation.hpp"
#include "Core.hpp"
#include "DataCollector.hpp"
#include "Enums.hpp"
#include "FCFSStation.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <fmt/core.h>
#include <regex>

void IOStation::ProcessArrival(Event &evt)
{
    evt.ServiceTime = _serviceTime();
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
    : _serviceTime(VariableStream(5,
                                  [this](auto rng) {
                                      if (_stationIndex == IO_1)
                                          return Exponential(SystemParameters::Parameters().averageIO1);
                                      else if (_stationIndex == IO_2)
                                          return Exponential(SystemParameters::Parameters().averageIO2);
                                      panic(fmt::format("Index {} is not right for this station", _stationIndex));
                                      return 0.0;
                                  })),
      FCFSStation(scheduler, stationIndex == Stations::IO_1 ? "IO1" : "IO2", stationIndex)
{

    _name = _stationIndex == 4 ? "IO1" : "IO2";
}
