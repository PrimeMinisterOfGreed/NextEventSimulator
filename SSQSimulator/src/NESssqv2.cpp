#include "NESssqv2.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Options.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include "rvms.h"
#include <cstdint>
#include <ostream>
#include <string>

NESssq::NESssq()
    : Scheduler("scheduler"), _serviceTimes(VariableStream{0, [](auto generator) { return Poisson(0.1); }}),
      _interArrivals(VariableStream{1, [](RandomStream generator) { return Poisson(0.1); }})
{
    Scheduler::_stations.push_back(sptr<Station>(new FCFSStation{this, "server", 1}));
}

void NESssq::Initialize()
{
    RandomStream::Global().PlantSeeds(123456789);
}

void NESssq::Execute()
{
    while (!_end)
    {
        auto nextEvt = Create(_interArrivals(), _serviceTimes());
        Schedule(nextEvt);
        auto inProcess = _eventList.Dequeue();
        Route(inProcess);
    }
}

void NESssq::Reset()
{
    Station::Reset();
}
