#pragma once

#include "Shell/SimulationShell.hpp"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
struct SystemParameters
{
    enum CPUMODE
    {
        HYPER_EXP,
        NEG_EXP,
        FIXED
    };
    double workStationThinkTime = 5000;
    double averageSwapIn = 210;
    double cpuQuantum = 2.7;
    double averageIO1 = 40;
    double averageIO2 = 180;
    double alpha = 0.95;
    double beta = 0.05;
    double u1 = 10;
    double u2 = 350;
    std::vector<double> cpuChoice{0.65, 0.25, 0.1, 0.0};
    double swapOutChoice[2] = {0.4, 0.6};
    int multiProgrammingDegree = 10;
    int numclients = 20;
    int burstmode = HYPER_EXP;
    int slicemode= FIXED;

    static SystemParameters &Parameters()
    {
        static SystemParameters instance = SystemParameters{};
        return instance;
    }

    void AddControlCommands(SimulationShell *shell)
    {
        shell->AddCommand("env", [](SimulationShell *s, const char *context) {
            auto p = SystemParameters::Parameters();
            s->Log()->Information("Z={}\nMPD={}\nN={}\navgSwaping={}", p.workStationThinkTime, p.multiProgrammingDegree,
                                  p.numclients, p.averageSwapIn);
        });
    }
};
