#include "MachineRepairman.hpp"
#include "DataCollector.hpp"
#include "DelayStation.hpp"
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
    : Scheduler("Scheduler"), _clientsDelay(1, [](auto rand) { return Exponential(400); }),
      _serviceTimes(2, [](auto rand) { return Exponential(15); })
{
    AddStation(DelayStation(this, "delay_station", 10, [this]() { return _clientsDelay(); }));
    AddStation(RepairStation(this, "repair_station", 1));
    _logger.verbosity = 0;
}

void MachineRepairman::Initialize()
{
    (*this)["delay_station"]->get()->Initialize();
    Schedule(Event("MAINTENANCE", MAINTENANCE, 0, _nominalWorkshift, _nominalRests, 0, 1));
    Schedule(Event("PROBE1", PROBE, 0, _nominalWorkshift, 0, 0, -1));
    Schedule(Event("PROBE2", PROBE, 0, _nominalWorkshift + _nominalRests, 0, 0, -1));
    Schedule(Event("END", END, 0, _nominalWorkshift + _nominalRests, 0, 0, -1));
}

void MachineRepairman::Execute()
{
    while (!_end)
    {
        auto inProcess = _eventList.Dequeue();
        Process(inProcess);
        _clock = inProcess.OccurTime;

        if (inProcess.Station == -1) // directed to schedule direct to first station
        {
            inProcess.Station = 1;
        }

        Route(inProcess);
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

void MachineRepairman::ProcessProbe(Event &evt)
{
    for (auto stat : _stations)
    {
        stat->Process(evt);
    }
}
