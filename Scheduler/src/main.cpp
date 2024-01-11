#include "DataWriter.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

std::vector<Accumulator<double>> _acc{};
std::vector<std::function<void()>> _collectFunctions{};
std::unique_ptr<OS> os;
std::unique_ptr<RegenerationPoint> regPoint;
SimulationShell shell{};
TraceSource logger{"main", 4};
RegenerationPoint *point;
bool hot = false;
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
        _acc[t](station->throughput());
        _acc[u](station->utilization());
        _acc[n](station->mean_customer_system());
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
                      m.confidence(0.90).precision(), m.Count(), m.confidence(0.90).lower(),
                      m.confidence(0.90).higher(), m.Current());
    }
}

void Setup()
{
    regPoint = std::unique_ptr<RegenerationPoint>(new RegenerationPoint(os.get(), os.get()));
    os->GetStation("SWAP_OUT").value()->OnDeparture([](auto s, auto e) { regPoint->Trigger(); });

    regPoint->AddRule(
        [](RegenerationPoint *pt) { return pt->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule(
        [](RegenerationPoint *pt) { return pt->scheduler->GetStation("IO2").value()->sysClients() == 2; });

    regPoint->AddAction([](RegenerationPoint *point) {
        CollectMeasures();
        point->scheduler->Reset();
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
    os = std::unique_ptr<OS>(new OS());
    shell.SetControllers(os.get(), os.get());
    for (auto m : _acc)
    {
        m.Reset();
    }
    Setup();
}

void SetupCommands()
{
    shell.AddCommand("lmeasures", [](SimulationShell *shell, auto c) { LogMeasures(); });
    shell.AddCommand("hreset", [](SimulationShell *shell, auto ctx) mutable { HReset(); });
    shell.AddCommand("exit", [](auto s, auto c) { exit(0); });
    shell.AddCommand("regPointStats", [](SimulationShell *s, auto c) {
        logger.Information("Regeneration point -> Hitted:{}, Called:{}", regPoint->hitted(), regPoint->called());
    });
    SystemParameters::Parameters().AddControlCommands(&shell);

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
            params.cpuQuantum = 2700;
            params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
            break;
        }
        HReset();
    });
    shell.AddCommand("lstats", [](auto s, auto c) {
        for (auto s : os->GetStations())
        {
            os->Sync();
            s->Update();
            logger.Result("S:{},B:{},O:{},A:{},S:{},N:{}", s->Name(), s->busyTime(), s->observation(), s->arrivals(),
                          s->completions(), s->sysClients());
        }
    });
    shell.AddCommand("lqueue", [](auto s, auto c) { logger.Result("Scheduler Queue:{}", os->EventQueue()); });
}

int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    RandomStream::Global().PlantSeeds(123456789);
    os = std::unique_ptr<OS>(new OS());
    shell.SetControllers(os.get(), os.get());
    Setup();
    SetupCommands();
    shell.Execute();
}
