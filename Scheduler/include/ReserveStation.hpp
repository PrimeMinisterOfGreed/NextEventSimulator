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
