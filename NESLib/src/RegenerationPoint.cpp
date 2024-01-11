#include "Strategies/RegenerationPoint.hpp"

RegenerationPoint::RegenerationPoint(IScheduler *sched, ISimulator *simulator) : simulator(simulator), scheduler(sched)
{
}

void RegenerationPoint::Trigger()
{
    _called++;
    for (auto r : _rules)
    {
        if (!r(this))
        {
            return;
        }
    }
    for (auto a : _actions)
    {
        a(this);
    }
    _hitted++;
}