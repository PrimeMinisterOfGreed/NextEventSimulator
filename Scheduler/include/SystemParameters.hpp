#pragma once

#include "Shell/SimulationShell.hpp"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
struct SystemParameters
{
    double workStationThinkTime = 5000;
    double averageSwapIn = 210;
    double cpuQuantum = 2.7;
    double averageIO1 = 40;
    double averageIO2 = 180;
    std::vector<double> cpuChoice{0.65, 0.25, 0.1, 0.0};
    double swapOutChoice[2] = {0.4, 0.6};
    int multiProgrammingDegree = 10;
    int numclients = 20;
    bool cpuUseNegExp = false;

    static SystemParameters &Parameters()
    {
        static SystemParameters instance = SystemParameters{};
        return instance;
    }

    void AddControlCommands(SimulationShell *shell)
    {
        shell->AddCommand("set", [](SimulationShell *s, const char *context) {
            char buffer[64]{};
            char argument[32]{};
            std::stringstream stream{context};
            stream.getline(buffer, 64, ' ').get(argument, 32, ' ');
            double arg = atof(argument);
            if (strcmp(buffer, "mpd") == 0)
                SystemParameters::Parameters().multiProgrammingDegree = arg;
            else if (strcmp(buffer, "N") == 0)
                SystemParameters::Parameters().numclients = arg;
            else if (strcmp(buffer, "stime") == 0)
                SystemParameters::Parameters().averageSwapIn = arg;
            else
                s->Log()->Exception("Variable {} Not found", buffer);
            s->Log()->Information("Set Variable {} to {}", buffer,arg);
        });

        shell->AddCommand("env", [](SimulationShell *s, const char *context) {
            auto p = SystemParameters::Parameters();
            s->Log()->Information("Z={}\nMPD={}\nN={}\navgSwaping={}", p.workStationThinkTime, p.multiProgrammingDegree,
                                  p.numclients, p.averageSwapIn);
        });
    }
};
