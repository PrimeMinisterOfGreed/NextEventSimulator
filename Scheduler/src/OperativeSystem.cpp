#include "OperativeSystem.hpp"
#include "CPU.hpp"
#include "DataWriter.hpp"
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

OS::OS()
    : Scheduler("OS"),
      _interArrival(
          VariableStream(1, [](auto &rng) { return Exponential(SystemParameters::Parameters().workStationThinkTime); }))
{
    static Cpu cpu(this);
    static ReserveStation rstation(this);
    static IOStation io1(this, Stations::IO_1);
    static IOStation io2(this, Stations::IO_2);
    static SwapOut swapOut(this);
    static SwapIn swapin(this);
    _stations = std::vector<sptr<Station>>({sptr<Station>(&rstation), sptr<Station>(&io1), sptr<Station>(&io2),
                                            sptr<Station>(&swapOut), sptr<Station>(&swapin)});
}

void OS::ProcessArrival(Event &evt)
{
    evt.Station = Stations::RESERVE_STATION;
    Schedule(evt);
    auto newEvt = Event(makeformat("J{}:S{}", Event::GeneratedNodes, 0), EventType::ARRIVAL, _clock,
                        _interArrival() + _clock, 0, _interArrival() + _clock, 0);
    Schedule(newEvt);
}

void OS::Reset()
{

    _end = false;
    for (auto station : _stations)
    {
        station->Reset();
    }
    _eventList.Clear();
}

void OS::Initialize()
{

    /*Schedule(Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION), EventType::ARRIVAL,
       _clock, 0, 0, 0, Stations::RESERVE_STATION));*/
}

void OS::ProcessProbe(Event &evt)
{
    Scheduler::ProcessProbe(evt);
}
