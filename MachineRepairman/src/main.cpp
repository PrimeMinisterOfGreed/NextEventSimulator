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
#include <regex>
#include <vector>

TraceSource _logger{""};

std::vector<Accumulator<double>> _acc{
    Accumulator<double>{"failedMachines", "unit"},
    Accumulator<double>{"repairedMachines", "unit"},
    Accumulator<double>("long_repair_avg", "min"),
    Accumulator<double>("avg_clients", "unit"),
};

bool ShouldStop()
{
    if (std::any_of(_acc.begin(), _acc.end(), [](Accumulator<double> &acc) { return acc.Count() < 40; }))
        return false;
    return _acc[2].confidence().precision() < 0.05;
};

void CollectStat(MachineRepairmanv2 &simulator)
{
    _acc[0](simulator["delay_station"].value()->completions());
    _acc[1](simulator["delay_station"].value()->arrivals());
    _acc[2](simulator["long_repair"].value()->Data()["avgWaiting"].value()->Current());
    _acc[3](simulator["long_repair"].value()->Data()["meanCustomerInSystem"].value()->Current());
    _logger.Information("Current Precision:{}, Current Mean: {}", _acc[2].confidence().precision(), _acc[2].mean());
};

void ExecuteRun()
{
    static int seed = 123456789;
    RandomStream::Global().PlantSeeds(seed);
    MachineRepairmanv2 simulator{};

    simulator["long_repair"].value()->OnDeparture([&simulator](auto s, Event &evt) {
        simulator.Update();
        CollectStat(simulator);
        if (ShouldStop())
            simulator.Stop();
        else
            simulator.Reset();
    });

    simulator.Initialize();
    simulator.Execute();

    seed++;
}

int main()
{
    LogEngine::CreateInstance("machinerepairman.txt");
    _logger = TraceSource{"main"};
    ExecuteRun();
    _logger.Result("Average waiting at long {}, precision {}", _acc[2].mean(), _acc[2].confidence().precision());
    _logger.Result("Average customers at long {}, precision {}", _acc[3].mean(), _acc[3].confidence().precision());
    LogEngine::Instance()->Flush();
}