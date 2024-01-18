#pragma once
#include "Measure.hpp"
#include "MvaSolver.hpp"
#include "Shell/SimulationShell.hpp"
#include <map>
#include <string>
#include <vector>

struct ConfidenceHits
{
    int throughput = 0;
    int utilization = 0;
    int meanwaits = 0;
    int meanclients = 0;

    bool throughput_in = false;
    bool utilization_in = false;
    bool meanClients_in = false;
    bool meanWaits_in = false;
    void Accumulate(bool x_in, bool u_in, bool n_in, bool w_in);
};

struct SimulationResult
{
    MVASolver mva{};

    std::map<std::string, ConfidenceHits> _confidenceHits{};
    std::vector<int> seeds;
    std::map<std::string, Accumulator<>[4]> _means;
    void AccumulateResult(std::map<std::string, Accumulator<>[4]> accumulators, int seed);
    SimulationResult();
    void AddShellCommands(SimulationShell *shell);
};