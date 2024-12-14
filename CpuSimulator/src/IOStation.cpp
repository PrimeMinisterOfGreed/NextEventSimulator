/**
 * @file IOStation.cpp
 * @author matteo.ielacqua
 * @brief implementazione di un IO, questa classe specializza una FCFSStation per integrarla nel 
 * sistema del progetto
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "IOStation.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "FCFSStation.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"
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
    evt.Type = EventType::ARRIVAL;
    _scheduler->Schedule(evt);
}
/**
 * @brief costruisce una stazione di IO, in base all'indice assegnato 
 * configura lo stream object con la giusta distribuzione negativa esponenziale
 * 
 * @param scheduler 
 * @param stationIndex 
 */
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
