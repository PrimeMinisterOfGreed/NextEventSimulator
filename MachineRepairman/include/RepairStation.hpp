#pragma once
#include "FCFSStation.hpp"

class RepairStation : public FCFSStation
{
  protected:
    bool _maintenance = false;
    void ProcessMaintenance(Event *evt) override;
    double _maintenanceEnd = 0.0;
  public:
    void Reset() override;
    void Initialize() override;
    void ProcessDeparture(Event *evt) override;
    RepairStation(ILogEngine * logger, IScheduler* scheduler,int stationIndex);
};