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
    Accumulator<> _meanTimes{"meanTimes", "ms"};
    Accumulator<> _regCycle{"regCycleTime", "ms"};

    TaggedCustomer()
    {
    }

    TaggedCustomer &WithRegPoint(RegenerationPoint *regPoint)
    {
        regPoint->AddAction([this](RegenerationPoint *) { CompleteRegCycle(); });
        return *this;
    }

    std::map<std::string, std::pair<double, double>> _times;
    void ConnectEntrance(BaseStation *station);
    void ConnectLeave(BaseStation *station);
    void AddShellCommands(SimulationShell *shell);
    void CompleteRegCycle();
};