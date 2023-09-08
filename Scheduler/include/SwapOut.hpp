//
// Created by drfaust on 23/01/23.
//
#pragma once

#include "ISimulator.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <memory>

class SwapOut : public Station
{
  protected:
    IScheduler *_scheduler;
    std::unique_ptr<VariableStream> _swap;

  public:
    SwapOut(IScheduler *scheduler);

  protected:
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};