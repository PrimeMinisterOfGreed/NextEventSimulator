#include "Event.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include <DelayStation.hpp>

void DelayStation::Initialize()
{
    Station::Initialize();
    for (int i = 0; i < _numclients; i++)
    {
        auto evt = Event(fmt::format("{}", Event::GeneratedNodes), ARRIVAL, _clock, _delayTime(), 0, 0, -1);
        _scheduler->Schedule(evt);
    }
}

void DelayStation::ProcessArrival(Event &evt)
{
    Station::ProcessArrival(evt);
    auto newevt = Event(fmt::format("{}", Event::GeneratedNodes), ARRIVAL, _clock, _clock + _delayTime(), 0, 0, -1);
    _scheduler->Schedule(newevt);
}
