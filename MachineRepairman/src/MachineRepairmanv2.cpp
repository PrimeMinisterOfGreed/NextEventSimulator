#include "MachineRepairmanv2.hpp"
#include "Core.hpp"
#include "DelayStation.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "RepairStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <fmt/core.h>
#include <sched.h>

MachineRepairmanv2::MachineRepairmanv2() : Scheduler("Scheduler")
{
    auto delay_station = new DelayStation(
        this, "delay_station",
        []() {
            static VariableStream delay(1, [](auto rng) { return Exponential(3000); });
            return delay();
        },
        [] { return 10; });

    delay_station->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 1;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });

    AddStation(delay_station);

    auto srepstation = new RepairStation(this, "short_repair", 1);
    srepstation->OnArrival([](auto s, Event &evt) {
        if (evt.SubType == MAINTENANCE)
            return;
        static VariableStream repair_time(3, [evt](auto rng) { return Exponential(40); });
        evt.ServiceTime = repair_time();
    });

    srepstation->OnDeparture([this](auto stat, Event &evt) {
        static Router router(
            3, {0.8, 0.2},
            {GetStation("delay_station").value()->stationIndex(), GetStation("long_repair").value()->stationIndex()});
        evt.Station = router();
        evt.Type = ARRIVAL;
        Schedule(evt);

    });
    AddStation(srepstation);

    auto lrepstation = new RepairStation(this, "long_repair", 2);
    lrepstation->OnArrival([](auto s, Event &evt) {
        static VariableStream longRepair{4, [](auto rng) { return Exponential(960); }};
        evt.ServiceTime = longRepair();
    });
    lrepstation->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 0;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });
    AddStation(lrepstation);

  
}

void MachineRepairmanv2::Initialize()
{
    (*this)["delay_station"].value()->Initialize();
}

void MachineRepairmanv2::Execute()
{
    ProcessNext();
}

void MachineRepairmanv2::ProcessEnd(Event &evt)
{
    _end = true;
}

void MachineRepairmanv2::ProcessArrival(Event &evt)
{
}

void MachineRepairmanv2::ProcessDeparture(Event &evt)
{
}

void MachineRepairmanv2::Stop()
{
    _end = true;
}

void MachineRepairmanv2::Update()
{
}
