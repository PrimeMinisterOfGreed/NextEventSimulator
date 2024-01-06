#include "NESssqv2.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include "rvms.h"
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

NESssq::NESssq()
    : Scheduler("scheduler"), _serviceTimes(VariableStream{2, [](auto generator) { return Exponential(1 / 0.14); }}),
      _interArrivals(VariableStream{1, [](RandomStream generator) { return Exponential(1 / 0.1); }})
{
    Scheduler::_stations.push_back(sptr<Station>(new FCFSStation{this, "server", 1}));
}

void NESssq::Initialize()
{
    RandomStream::Global().PlantSeeds(123456789);
    auto nextEvt = Create(0.0, _serviceTimes());
    _logger.Information("Created:{}", nextEvt);
    Schedule(nextEvt);
}

void NESssq::Execute()
{
    while (!_end)
    {
        auto ref = (*this)["server"].value();
        _logger.Transfer("Schedule Queue:{}", _eventList);
        _logger.Transfer("Server Queue:{}", std::static_pointer_cast<FCFSStation>(ref)->GetEventList());
        auto inProcess = _eventList.Dequeue();
        Process(inProcess);

        inProcess.Station = 1;
        _clock = inProcess.OccurTime;
        Route(inProcess);

        if (ref->arrivals() == ref->completions() && _clock > 1000)
        {
            ProcessEnd(inProcess);
            return;
        }
    }
}

void NESssq::Reset()
{
}

void NESssq::ProcessEnd(Event &evt)
{
    _end = true;
    _logger.Result("End of simulation time {}", _clock);
    _logger.Result("Arrived customers:{}", _stations.at(0)->arrivals());
    _logger.Result("Departures:{}", _stations.at(0)->completions());
}

void NESssq::ProcessArrival(Event &evt)
{
    Scheduler::ProcessArrival(evt);
    auto nextEvt = Create(_interArrivals(), _serviceTimes());
    _logger.Information("Created:{}", nextEvt);
    Schedule(nextEvt);
}
