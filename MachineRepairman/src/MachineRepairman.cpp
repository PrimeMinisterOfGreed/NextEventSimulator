#include "MachineRepairman.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "RepairStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "ToString.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <gtest/gtest.h>
#include <vector>

MachineRepairman::MachineRepairman()
    : Scheduler("DelayStation"), _interArrival(1, [](auto rand) { return Exponential(400); }),
      _serviceTimes(1, [](auto rand) { return Exponential(15); })
{
}
