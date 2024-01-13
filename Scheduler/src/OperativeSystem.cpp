#include "OperativeSystem.hpp"
#include "CPU.hpp"
#include "Core.hpp"
#include "DataWriter.hpp"
#include "DelayStation.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "IOStation.hpp"
#include "ReserveStation.hpp"
#include "Scheduler.hpp"
#include "SwapIn.hpp"
#include "SwapOut.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <fmt/core.h>

void OS::Execute()
{
    auto nextEvt = _eventList.Dequeue();
    _clock = nextEvt.OccurTime;
    Process(nextEvt);
    Route(nextEvt);
    if (_eventList.Count() == 0)
    {
        panic(fmt::format("Error eventlist is empty, last event processed {}", nextEvt));
    }
}

OS::OS() : Scheduler("OS")
{
    DelayStation *dstation = new DelayStation(
        this, "delay_station",
        [dstation]() {
            static VariableStream delayStream(
                1, [](auto rng) { return Exponential(SystemParameters::Parameters().workStationThinkTime); });
            return delayStream();
        },
        []() { return SystemParameters::Parameters().numclients; });

    dstation->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 1;
        evt.Type = ARRIVAL;
        Schedule(evt);
    });
    AddStation(dstation);
    AddStation(new SwapOut(this));
    AddStation(new Cpu(this));
    AddStation(new ReserveStation(this));
    AddStation(new IOStation(this, Stations::IO_1));
    AddStation(new IOStation(this, Stations::IO_2));
    AddStation(new SwapIn(this));
}

void OS::ProcessArrival(Event &evt)
{
}

void OS::Reset()
{

    _end = false;
    for (auto station : _stations)
    {
        station->Reset();
    }
}

void OS::Initialize()
{
    (*this)["delay_station"].value()->Initialize();
}

void OS::ProcessProbe(Event &evt)
{
    Scheduler::ProcessProbe(evt);
}
