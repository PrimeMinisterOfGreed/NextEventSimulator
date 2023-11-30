#pragma once
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <vector>

class MachineRepairman : public Scheduler, public ISimulator
{
  private:
  public:
};