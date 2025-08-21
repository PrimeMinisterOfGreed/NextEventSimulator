#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "MachineRepairmanv2.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <algorithm>
#include <cmath>
#include <fmt/core.h>
#include <regex>
#include <vector>

TraceSource _logger{""};

CovariatedMeasure acc{"lrepwaittimes", "min"};
CovariatedMeasure nclient{"nclients", "unit"};
bool ShouldStop()
{
    return acc.confidence().precision() < 0.05 && acc.Count() > 40 && nclient.confidence().precision() < 0.05;
};

void CollectStat(MachineRepairmanv2 &simulator)
{
    auto lrep = simulator.GetStation("long_repair").value();
    acc(lrep->areaN(), lrep->completions());
    nclient(lrep->areaN(), simulator.GetClock() - nclient.times());
};

void ExecuteRun()
{
    static int seed = 123456789;
    RandomStream::Global().PlantSeeds(seed);
    MachineRepairmanv2 simulator{};
    RegenerationPoint regPoint{&simulator, &simulator};

    simulator.GetStation("long_repair").value()->OnDeparture([&simulator, &regPoint](auto s, auto e) {
        regPoint.Trigger();
    });

    regPoint.AddRule([](RegenerationPoint *pt) {
        auto srepair = pt->scheduler->GetStation("short_repair").value();
        auto dstat = pt->scheduler->GetStation("delay_station").value();
        auto lrep = pt->scheduler->GetStation("long_repair").value();
        return srepair->sysClients() == 0 && lrep->sysClients() == 5;
    });
    regPoint.AddAction([&simulator](RegenerationPoint *pt) {
        for (auto s : simulator.GetStations())
        {
            fmt::println("S:{},N:{},A:{},C:{}", s->Name(), s->sysClients(), s->arrivals(), s->completions());
        }
        CollectStat(simulator);
        fmt::println("Measure:{}", acc);
        fmt::println("Measure:{}", nclient);
        simulator.Reset();
    });

    simulator.Initialize();
    while (!ShouldStop())
    {
        simulator.Execute();
    }

    seed++;
}

int main()
{
    LogEngine::CreateInstance("machinerepairman.txt");
    acc.WithConfidence(0.95).WithPrecision(0.05);
    nclient.WithConfidence(0.95).WithPrecision(0.05);
    _logger = TraceSource{"main"};
    ExecuteRun();
    LogEngine::Instance()->Flush();
}