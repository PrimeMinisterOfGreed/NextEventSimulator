#pragma once

#include "Event.hpp"
#include "ISimulator.hpp"
#include "MachineRepairman.hpp"
#include "Scheduler.hpp"
class MachineRepairmanv2 : public Scheduler
{
  private:
    const int _nominalWorkshift = 480;
    const int _nominalRests = 960;
    bool _end = false;

  public:
    MachineRepairmanv2();
    void Initialize() override;
    virtual void Execute();
    virtual void ProcessEnd(Event &evt) override;
    virtual void ProcessArrival(Event &evt) override;
    virtual void ProcessDeparture(Event &evt) override;
    void Stop();
    void Update();
};