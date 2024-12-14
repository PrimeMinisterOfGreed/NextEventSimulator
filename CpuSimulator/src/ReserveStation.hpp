/**
 * @file ReserveStation.hpp
 * @author header della classe ReserveStation
 * @see ReserveStation.cpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "FCFSStation.hpp"
#include "rngs.hpp"

class ReserveStation : public FCFSStation
{
  protected:
    int _multiProgrammingDegree;
    int _counter = 0;

  public:
    void ProcessDeparture(Event &evt) override;

    void ProcessArrival(Event &evt) override;

    ReserveStation(IScheduler *scheduler);
};
