#pragma once
#include "FCFSStation.hpp"

class ReserveStation : public FCFSStation
{
  protected:
    int _multiProgrammingDegree;

  public:
    void ProcessDeparture(Event &evt) override;

    void ProcessArrival(Event &evt) override;

    ReserveStation(IScheduler *scheduler);
};
