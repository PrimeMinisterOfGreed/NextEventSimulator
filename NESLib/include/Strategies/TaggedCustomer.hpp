#pragma once
#include "Event.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include <map>
#include <utility>
struct TaggedCustomer
{
    CovariatedMeasure _mean{"cycleTime", "ms"};
    Accumulator<> _acc{"regTime", "ms"};
    std::string target_client = "";
    TaggedCustomer()
    {
    }

    TaggedCustomer &WithRegPoint(RegenerationPoint *regPoint)
    {
        regPoint->AddAction([this](RegenerationPoint *pt) { CompleteRegCycle(pt->scheduler->GetClock()); });
        return *this;
    }

    double time = 0;
    void ConnectEntrance(BaseStation *station, bool arrival = false);
    void ConnectLeave(BaseStation *station, bool arrival = false);
    void CompleteRegCycle(double actualClock);
    void CompleteSimulation();
};