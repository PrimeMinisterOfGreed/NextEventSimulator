#include "DataWriter.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include <fmt/core.h>
#include <vector>

std::vector<Accumulator<double>> _acc{

    Accumulator<double>{"CPU_throughput", "j/s"},
    Accumulator<>{"CPU_utilization", ""},
    Accumulator<>{"CPU_meanclients", ""},

};
OS os;
SimulationShell shell{&os, &os};
TraceSource logger{"main"};

void CollectMeasures()
{
    auto stat = os["CPU"].value();
    stat->Update();
    _acc[0](stat->Data()["throughput"].value()->Current());
    _acc[1](stat->Data()["utilization"].value()->Current());
    _acc[2](stat->Data()["meanCustomerInSystem"].value()->Current());
}

void LogMeasures()
{
    for (auto m : _acc)
    {
        logger.Result("Measure: {}, Mean: {}, Precision:{}, Samples:{}, LB:{}, LH:{}", m.Name(), m.mean(),
                      m.confidence().precision(), m.Count(), m.confidence().lower(), m.confidence().higher());
    }
}

void Setup()
{
    os["SWAP_OUT"].value()->OnDeparture([](auto s, auto e) {
        os.Sync();
        CollectMeasures();
        os.Reset();
        LogMeasures();
    });

    shell.AddCommand("list", [](SimulationShell *shell, auto c) { LogMeasures(); });
}

int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    Setup();
    shell.Execute();
}
