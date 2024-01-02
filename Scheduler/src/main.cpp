#include "DataWriter.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "SystemParameters.hpp"
#include <cstdlib>
#include <fmt/core.h>
#include <sstream>
#include <vector>

std::vector<Accumulator<double>> _acc{

    Accumulator<double>{"CPU_throughput", "j/s"},
    Accumulator<>{"CPU_utilization", ""},
    Accumulator<>{"CPU_meanclients", ""},
    Accumulator<>{"IO2_Throughput", ""},
    Accumulator<>{"IO2_utilization", ""},
    Accumulator<>{"IO2_meanclients", ""}

};
OS *os;
SimulationShell shell{};
TraceSource logger{"main"};

void CollectMeasures()
{
    auto cpu = os->GetStation("CPU").value();
    auto io2 = os->GetStation("IO2").value();
    cpu->Update();
    io2->Update();
    _acc[0](cpu->Data()["throughput"].value()->Current());
    _acc[1](cpu->Data()["utilization"].value()->Current());
    _acc[2](cpu->Data()["meanCustomerInSystem"].value()->Current());

    _acc[3](io2->Data()["throughput"].value()->Current());
    _acc[4](io2->Data()["utilization"].value()->Current());
    _acc[5](io2->Data()["meanCustomerInSystem"].value()->Current());
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
    os->GetStation("SWAP_OUT").value()->OnDeparture([](auto s, auto e) {
        os->Sync();
        CollectMeasures();
        os->Reset();
        LogMeasures();
    });
}

void HReset()
{
    os = new OS();
    shell.SetControllers(os, os);
    for (auto m : _acc)
    {
        m.Reset();
    }
    Setup();
}

void SetupCommands()
{
    shell.AddCommand("list", [](SimulationShell *shell, auto c) { LogMeasures(); });
    shell.AddCommand("hreset", [](SimulationShell *shell, auto ctx) mutable { HReset(); });
    shell.AddCommand("exit", [](auto s, auto c) { exit(0); });

    shell.AddCommand("scenario", [](auto s, auto c) {
        std::stringstream stream{c};
        char buf[12]{};
        stream.get(buf, 12, ' ');
        int val = atoi(buf);
        auto &params = SystemParameters::Parameters();
        switch (val)
        {
        case 1:
            params.cpuUseNegExp = false;
            params.cpuQuantum = 2.7;
            break;

        case 2:
            params.cpuUseNegExp = true;
            params.cpuQuantum = 2.7;
            break;

        case 3:
            params.cpuUseNegExp = false;
            params.cpuQuantum = 2700;
            break;

        case 4:
            params.cpuUseNegExp = true;
            params.cpuQuantum = 2700;
            break;

        case 5:
            params.cpuUseNegExp = true;
            params.cpuQuantum = 27;
            params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
            break;
        }
    });
    SystemParameters::Parameters().AddControlCommands(&shell);
}

int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    os = new OS();
    shell.SetControllers(os, os);
    Setup();
    SetupCommands();
    shell.Execute();
}
