#include "Strategies/RegenerationPoint.hpp"

void RegenerationPoint::Trigger()
{
    _called++;
    for (auto r : _rules)
    {
        if (!r(scheduler, simulator))
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