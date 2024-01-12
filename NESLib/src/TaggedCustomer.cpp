#include "Core.hpp"
#include "Event.hpp"
#include "Shell/SimulationShell.hpp"
#include <Strategies/TaggedCustomer.hpp>
#include <fmt/core.h>

void TaggedCustomer::ConnectEntrance(BaseStation *station)
{
    station->OnArrival([this](auto s, Event &e) { _times[e.Name] = {e.OccurTime, 0}; });
}

void TaggedCustomer::ConnectLeave(BaseStation *station)
{
    station->OnDeparture([this](auto s, Event &e) {
        if (_times.count(e.Name) == 0)
            panic(fmt::format("Event with name {} not registered at entrance", e.Name));
        _times[e.Name].second = e.OccurTime;
        auto pair = _times.at(e.Name);
        _regCycle(pair.second - pair.first);
    });
}

void TaggedCustomer::AddShellCommands(SimulationShell *shell)
{
    shell->AddCommand("ltgtstats",
                      [this](SimulationShell *s, auto ctx) { s->Log()->Result("{}\n{}", _meanTimes, _regCycle); });
}

void TaggedCustomer::CompleteRegCycle()
{
    _meanTimes(_regCycle.mean());
    _regCycle.Reset();
}