#pragma once

#include "Event.hpp"
#include "ISimulator.hpp"
#include "MachineRepairman.hpp"
#include "Scheduler.hpp"
class MachineRepairmanv2 : public Scheduler
{
  private:
    bool _end = false;

  public:
    MachineRepairmanv2();
    void Initialize() override;
    virtual void Execute();
    virtual void ProcessEnd(Event &evt) override;
    void Stop();
    void Update();
};