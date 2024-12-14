/**
 * @file RegenerationPoint.cpp
 * @author matteo.ielacqua
 * @see RegenerationPoint.hpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Strategies/RegenerationPoint.hpp"

RegenerationPoint::RegenerationPoint(IScheduler *sched, ISimulator *simulator) : simulator(simulator), scheduler(sched)
{
}

void RegenerationPoint::Trigger()
{
    _called++;
    if (_rulesEnabled)
    {
        for (auto r : _rules)
        {
            if (!r(this))
            {
                return;
            }
        }
    }
    for (auto a : _actions)
    {
        a(this);
    }
    for (auto a : _onTimeActions)
    {
        a(this);
    }
    _onTimeActions.clear();
    _hitted++;
}