#pragma once
#include "FCFSStation.hpp"

class RepairStation : public FCFSStation
{
  protected:
    bool _maintenance = false;
    NegExpVariable & _serviceVariable;
    void ProcessMaintenance(Event *evt) override;
  public:
    void Reset() override;
    void Initialize() override;
    void ProcessDeparture(Event *evt) override;
    RepairStation(ILogEngine * logger, IScheduler* scheduler, int stationIndex, IGenerator * stream);

    void ProcessArrival(Event *evt) override;
};