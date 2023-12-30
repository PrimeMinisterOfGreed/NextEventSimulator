#pragma once

struct SystemParameters
{
    double workStationThinkTime = 5000;
    double averageSwapIn = 210;
    double cpuQuantum = 2.7;
    double averageIO1 = 40;
    double averageIO2 = 180;
    double cpuChoice[3] = {0.65, 0.25, 0.1};
    double swapOutChoice[2] = {0.4, 0.6};
    int multiProgrammingDegree = 10;
    int numclients = 20;

    static SystemParameters &Parameters()
    {
        static SystemParameters instance = SystemParameters{};
        return instance;
    }
};
