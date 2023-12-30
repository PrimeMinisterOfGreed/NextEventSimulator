#include "OperativeSystem.hpp"
#include "CPU.hpp"
#include "DataWriter.hpp"
#include "DelayStation.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "IOStation.hpp"
#include "ISimulator.hpp"
#include "Options.hpp"
#include "ReserveStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "SwapOut.hpp"
#include "SystemParameters.hpp"
#include "Usings.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <vector>

void OS::Execute()
{
    Initialize();
    while (!_end)
    {
        auto nextEvt = _eventList.Dequeue();
        _clock = nextEvt.OccurTime;
        Process(nextEvt);
        Route(nextEvt);
    }
}

OS::OS() : Scheduler("OS")
{
    auto dstation = new DelayStation(this, "delay_station", 20, []() {
        static VariableStream delayStream(
            1, [](auto rng) { return Exponential(SystemParameters::Parameters().workStationThinkTime); });
        return delayStream();
    });

    dstation->OnDeparture([this](auto s, Event &evt) {
        evt.Station = 1;
        Schedule(evt);
    });
    AddStation(dstation);
    AddStation(new Cpu(this));
    AddStation(new ReserveStation(this));
    AddStation(new IOStation(this, Stations::IO_1));
    AddStation(new IOStation(this, Stations::IO_2));
    AddStation(new SwapOut(this));
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
