#include "MachineRepairman.hpp"
#include "DataCollector.hpp"
#include "DelayStation.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "RepairStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <vector>

MachineRepairman::MachineRepairman() : Scheduler("Scheduler")
{
    auto delay_station = new DelayStation(this, "delay_station", [this]() {
        static VariableStream delay{1, [](auto rand) { return Exponential(400); }};
        return delay();
    },[](){return 10;});

    delay_station->OnDeparture([this](auto s, Event &evt) {
        static VariableStream repairTime(2, [](auto rand) { return Exponential(15); });
        evt.ServiceTime = repairTime();
        evt.Station = 1;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });

    AddStation(delay_station);
    auto rstation = new RepairStation(this, "repair_station", 1);

    rstation->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 0;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });

    AddStation(rstation);
    _logger.verbosity = 0;
}

void MachineRepairman::Initialize()
{
    (*this)["delay_station"]->get()->Initialize();
    Schedule(Event("MAINTENANCE", MAINTENANCE, 0, _nominalWorkshift, _nominalRests, 0, 1));
    Schedule(Event("END", END, 0, _nominalWorkshift, 0, 0, -1));
}

void MachineRepairman::Execute()
{
    while (!_end)
    {
        auto inProcess = _eventList.Dequeue();
        Process(inProcess);
        _clock = inProcess.OccurTime;
        Route(inProcess);
    }
}

void MachineRepairman::Reset()
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
