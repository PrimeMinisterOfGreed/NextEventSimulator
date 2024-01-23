#include "Core.hpp"
#include "Event.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "rngs.hpp"
#include <Strategies/TaggedCustomer.hpp>
#include <algorithm>
#include <fmt/core.h>
#include <functional>

void TaggedCustomer::ConnectEntrance(BaseStation *station, bool arrival)
{
    auto l = [this](auto s, Event &e) { _times[e.Name] = {e.OccurTime, 0}; };
    if (arrival)
        station->OnArrival(l);
    else
        station->OnDeparture(l);
}

void TaggedCustomer::ConnectLeave(BaseStation *station, bool arrival)
{
    auto l = [this](auto s, Event &e) {
        if (_times.count(e.Name) == 0)
            panic(fmt::format("Event with name {} not registered at entrance", e.Name));
        _times[e.Name].second = e.OccurTime;
        auto pair = _times.at(e.Name);
        _regCycle(pair.second - pair.first);
    };
    if (arrival)
        station->OnArrival(l);
    else
        station->OnDeparture(l);
}

void TaggedCustomer::AddShellCommands(SimulationShell *shell)
{
    shell->AddCommand("ltgtstats", [this](SimulationShell *s, auto ctx) {
        s->Log()->Result("{}\n{}\n{}\n{}", _meanTimes, _antitetichMeanTimes, _regCycle, ComputeGrandMean());
    });
}

void TaggedCustomer::CompleteRegCycle()
{
    if (RandomStream::Global().antitethic)
        _antitetichMeanTimes(_regCycle.mean());
    else
        _meanTimes(_regCycle.mean());
    _regCycle.Reset();
}
Accumulator<> TaggedCustomer::ComputeGrandMean()
{
    Accumulator<> acc{"GrandMean", "ms"};
    for (int i = 0; i < std::min(_meanTimes.Count(), _antitetichMeanTimes.Count()); i++)
    {
        acc((_meanTimes.Data()[i] + _antitetichMeanTimes.Data()[i]) / 2);
    }
    return acc;
}
