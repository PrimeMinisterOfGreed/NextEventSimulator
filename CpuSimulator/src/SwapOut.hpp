/**
 * @file SwapOut.hpp
 * @author matteo.ielacqua
 * @see SwapOut.cpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "ISimulator.hpp"
#include "Station.hpp"

class SwapOut : public Station
{
  protected:
    IScheduler *_scheduler;

  public:
    SwapOut(IScheduler *scheduler);

  protected:
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};