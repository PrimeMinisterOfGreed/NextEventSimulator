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
      _serviceTimes(2, [](auto rand) { return Exponential(15); })
{
    AddStation(FCFSStation(this, "repair_station", 1));
}

void MachineRepairman::Initialize()
{
    Schedule(Event("MAINTENANCE", MAINTENANCE, 0, _nominalWorkshift, _nominalRests, 0, 0));
    Schedule(Create(0, 0));
}

void MachineRepairman::Execute()
{
    while (!_end)
    {
        auto inProcess = _eventList.Dequeue();
        Process(inProcess);

        inProcess.Station = 1;
        _clock = inProcess.OccurTime;
        Route(inProcess);
        if (_clock > _nominalWorkshift)
        {
            return;
        }
    }
}

void MachineRepairman::Reset()
{
}

void MachineRepairman::ProcessMaintenance(Event &evt)
{
}

void MachineRepairman::ProcessEnd(Event &evt)
{
    Scheduler::ProcessEnd(evt);
    _end = true;
}
