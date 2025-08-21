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
