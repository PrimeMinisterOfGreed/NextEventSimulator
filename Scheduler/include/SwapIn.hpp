
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
    std::unique_ptr<VariableStream> _serviceTime;

  public:
};
