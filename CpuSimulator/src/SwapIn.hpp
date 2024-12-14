/**
 * @file SwapIn.hpp
 * @author matteo.ielacqua
 * @see SwapIn.cpp
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

class SwapIn : public FCFSStation
{
  public:
    SwapIn(IScheduler *scheduler);

    void ProcessArrival(Event &evt) override;

    void ProcessDeparture(Event &evt) override;

  private:
    VariableStream _serviceTime;

  public:
};
