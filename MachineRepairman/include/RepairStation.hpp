#pragma once
#include "FCFSStation.hpp"
#include "ISimulator.hpp"

class RepairStation : public FCFSStation
{
  public:
    RepairStation(IScheduler *scheduler, std::string name, int index);
    void ProcessDeparture(Event &evt) override;
    void ProcessProbe(Event &evt) override;
};