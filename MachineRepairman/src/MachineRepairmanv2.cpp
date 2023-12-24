#include "MachineRepairmanv2.hpp"
#include "DelayStation.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "RepairStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include "rvgs.h"

MachineRepairmanv2::MachineRepairmanv2() : Scheduler("Scheduler")
{
    auto delay_station = new DelayStation(this, "delay_station", 10, []() {
        static VariableStream delay(1, [](auto rng) { return Exponential(3000); });
        return delay();
    });

    delay_station->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 1;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });

    AddStation(delay_station);

    auto srepstation = new RepairStation(this, "short_repair", 1);
    srepstation->OnArrival([](auto s, Event &evt) {
        static VariableStream repair_time(3, [evt](auto rng) { return Exponential(40); });
        evt.ServiceTime = repair_time();
    });

    srepstation->OnDeparture([this](auto stat, auto evt) {
        static CompositionStream router{2,
                                        {0.2, 0.8},
                                        [this, evt](auto s) {
                                            auto newEvt = Event(evt);
                                            newEvt.Type = ARRIVAL;
                                            newEvt.Station = (*this)["long_repair"].value()->stationIndex();
                                            Schedule(newEvt);
                                            return 1;
                                        },
                                        [this, evt](auto s) {
                                            auto newEvt = Event(evt);
                                            newEvt.Type = ARRIVAL;
                                            newEvt.Station = (*this)["delay_station"].value()->stationIndex();
                                            Schedule(newEvt);
                                            return 0;
                                        }};
        router();
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
    while (!_end)
    {
        auto inProcess = _eventList.Dequeue();
        Process(inProcess);
        _clock = inProcess.OccurTime;
        Route(inProcess);
    }
}

void MachineRepairmanv2::ProcessEnd(Event &evt)
{
    _end = true;
}

void MachineRepairmanv2::Stop()
{
    _end = true;
}

void MachineRepairmanv2::Update()
{
    for (auto s : _stations)
    {
        s->Update();
    }
}
