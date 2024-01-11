#pragma once
#include "Event.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include <map>
#include <utility>
struct TaggedCustomer
{
    Accumulator<> _meanTimes;
    std::map<std::string, std::pair<double, double>> _times;
    void ConnectEntrance(BaseStation * station);
    void ConnectLeave(BaseStation * station);
    void AddShellCommands(SimulationShell* shell);
};