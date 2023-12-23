#pragma once

#include "ISimulator.hpp"
#include "MachineRepairman.hpp"
#include "Scheduler.hpp"
class MachineRepairmanv2 : public MachineRepairman
{
  private:
  public:
    MachineRepairmanv2();
    void Initialize() override;
};