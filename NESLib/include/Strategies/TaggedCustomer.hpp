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
    EsembledMeasure<3> _mean{"cycleTime", "ms"};

    TaggedCustomer()
    {
    }

    TaggedCustomer &WithRegPoint(RegenerationPoint *regPoint)
    {
        regPoint->AddAction([this](RegenerationPoint *) { CompleteRegCycle(); });
        return *this;
    }

    std::map<std::string, std::pair<double, double>> _times;
    void ConnectEntrance(BaseStation *station, bool arrival = false);
    void ConnectLeave(BaseStation *station, bool arrival = false);
    void AddShellCommands(SimulationShell *shell);
    void CompleteRegCycle();
    void CompleteSimulation();
};