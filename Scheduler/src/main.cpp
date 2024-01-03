#include "DataWriter.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <sstream>
#include <vector>

std::vector<Accumulator<double>> _acc{};
std::vector<std::function<void()>> _collectFunctions{};
OS *os;
SimulationShell shell{};
TraceSource logger{"main"};
double regPoint = 0.0;
void AddStationToCollectibles(std::string name)
{
    using namespace fmt;
    auto t = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_throughput", name), "j/s"});
    auto u = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_utilization", name), ""});
    auto n = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_meanclients", name), ""});
    _collectFunctions.push_back([t, u, n, name] {
        auto station = os->GetStation(name).value();
        station->Update();
        _acc[t](station->Data()["throughput"].value()->Current());
        _acc[u](station->Data()["utilization"].value()->Current());
        _acc[n](station->Data()["meanCustomerInSystem"].value()->Current());
    });
}

void CollectMeasures()
{
    for (auto f : _collectFunctions)
    {
        f();
    }
}

void LogMeasures()
{
    for (auto m : _acc)
    {
        logger.Result("Measure: {}, Mean: {}, Precision:{}, Samples:{}, LB:{}, LH:{},LastValue:{}", m.Name(), m.mean(),
                      m.confidence().precision(), m.Count(), m.confidence().lower(), m.confidence().higher(),
                      m.Current());
    }
}

void Setup()
{
    os->GetStation("SWAP_OUT").value()->OnDeparture([](auto s, Event &e) {
        os->Sync();
        if (e.Station == 0)
        {
            CollectMeasures();
            os->Reset();
            LogMeasures();
            regPoint = e.OccurTime;
        }
    });
    _acc.clear();
    AddStationToCollectibles("CPU");
    AddStationToCollectibles("IO1");
    AddStationToCollectibles("IO2");
    AddStationToCollectibles("SWAP_IN");
    AddStationToCollectibles("SWAP_OUT");
    AddStationToCollectibles("RESERVE_STATION");
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
    shell.AddCommand("lstats", [](auto s, auto c) {
        for (auto s : os->GetStations())
        {
            os->Sync();
            s->Update();
            logger.Result("S:{},B:{},O:{},A:{},S:{}", s->Name(), s->busyTime(), s->observation(), s->arrivals(),
                          s->completions());
        }
    });
    shell.AddCommand("regClock", [](auto s, auto c) { logger.Result("RegClock:{}", os->GetClock() - regPoint); });
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
