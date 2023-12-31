
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
