/**
 * @file IOStation.hpp
 * @author header per la stazione di IO, maggiori informazioni @see IOStation.cpp
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "FCFSStation.hpp"
#include "rngs.hpp"
#include <memory>

class IOStation : public FCFSStation
{
  private:
    VariableStream _serviceTime;

  public:
    IOStation(IScheduler *scheduler, int stationIndex);
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};
