#include "MachineRepairmanv2.hpp"
#include "DelayStation.hpp"
#include "Event.hpp"
#include "MachineRepairman.hpp"
#include "RepairStation.hpp"
#include "Scheduler.hpp"
#include "rngs.hpp"
#include "rvgs.h"

MachineRepairmanv2::MachineRepairmanv2() : MachineRepairman()
{
    AddStation(new DelayStation(this, "delay_station", 10, []() {
        static VariableStream delay(1, [](auto rng) { return Exponential(3000); });
        return delay();
    }));

    auto srepstation = new RepairStation(this, "short_repair", 1);
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
    AddStation(lrepstation);
}

void MachineRepairmanv2::Initialize()
{
    (*this)["delay_station"].value()->Initialize();
}
