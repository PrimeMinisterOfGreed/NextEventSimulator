#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <cmath>
#include <vector>

Accumulator<double> _failedMachines{"failedMachines", "unit"};
Accumulator<double> _returnToWork{"repairedMachines", "unit"};

void ExecuteRun()
{
    static int seed = 123456789;
    RandomStream::Global().PlantSeeds(seed);
    MachineRepairman simulator{};
    simulator.Initialize();
    simulator.Execute();
    _failedMachines(simulator.arrivals());
    _returnToWork(simulator.completions());
    seed++;
}

int main()
{
    LogEngine::CreateInstance("machinerepairman.txt");
    TraceSource logger{"Main"};
    for (int i = 0; i < 50; i++)
    {
        ExecuteRun();
    }
    while (_failedMachines.confidence().precision() >= 0.10)
    {
        ExecuteRun();
    }
    logger.Result("{}", _failedMachines.Json());
    logger.Result("{}", _returnToWork.Json());
    LogEngine::Instance()->Flush();
}