#pragma once

static double operator""_ms(long double value)
{
    return value / 1000;
}

static double operator""_ms(unsigned long long value)
{
    return (double)value / 1000;
}

struct SystemParameters
{
    double workStationThinkTime = 5000_ms;
    double averageSwapIn = 210_ms;
    double averageCpuTime = 27_ms;
    double cpuQuantum = 2.7_ms;
    double averageIO1 = 40_ms;
    double averageIO2 = 180_ms;
    double cpuChoice[3] = {0.65, 0.25, 0.1};
    double swapOutChoice[2] = {0.4, 0.6};
    int multiProgrammingDegree = 10;
    double probeInterval = 1;
    double endTime = 1000;
    static SystemParameters &Parameters()
    {
        static SystemParameters instance = SystemParameters{};
        return instance;
    }
};
