#pragma once
struct SystemParameters
{
    double workStationThinkTime = 5000;
    double averageSwapIn = 210;
    double averageCpuTime = 27;
    double cpuQuantum = 2.7;
    double averageIO1 = 40;
    double averageIO2 = 180;
    double cpuChoice[3] = {0.65, 0.25, 0.1};
    double swapOutChoice[2] = {0.4, 0.6};
    int multiProgrammingDegree = 10;
    double probeInterval = 100;
    double endTime = 100000;
    static SystemParameters &Parameters()
    {
        static SystemParameters instance = SystemParameters{};
        return instance;
    }
};
