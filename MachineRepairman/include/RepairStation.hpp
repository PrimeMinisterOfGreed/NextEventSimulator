#pragma once
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"

class RepairStation : public FCFSStation
{
  public:
    RepairStation(IScheduler *scheduler, std::string name, int index);
    void ProcessProbe(Event &evt) override;
    virtual void ProcessMaintenance(Event& evt);
};
