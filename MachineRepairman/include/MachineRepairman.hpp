#pragma once
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <vector>

class MachineRepairman : public Scheduler, public ISimulator
{
  private:
    const int _nominalWorkshift = 480;
    const int _nominalRests = 960;
    bool _end = false;

  public:
    MachineRepairman();

    void Initialize() override;
    void Execute() override;
    void Reset() override;

    void ProcessMaintenance(Event &evt) override;
    void ProcessEnd(Event &evt) override;
    void ProcessProbe(Event &evt) override;
};