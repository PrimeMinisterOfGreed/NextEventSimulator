#include "OperativeSystem.hpp"
#include "Event.hpp"
#include "Generators.hpp"
#include "Enums.hpp"
#include "Options.hpp"

void OS::Execute()
{
    while (!_end)
    {
    }
}

void OS::Report()
{

}

OS::OS()
{

    Initialize();
}

void OS::Reset()
{
    _end = false;
}

void OS::RouteToStation(Event *evt)
{

}

void OS::Initialize()
{
    Schedule(new Event(makeformat("J{}:S{}", Event::GeneratedNodes, Stations::RESERVE_STATION), EventType::ARRIVAL,
                       _clock, 0, 0, 0, Stations::RESERVE_STATION));
}

void OS::Schedule(Event *event)
{
    _eventQueue.Insert(event, [](const Event &a, const Event &b)
    { return a.OccurTime > b.OccurTime; });
}
