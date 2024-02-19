#include "SimulationEnv.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Measure.hpp"
#include "MvaSolver.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "SimulationResult.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include "Usings.hpp"
#include "rngs.hpp"
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <fmt/core.h>
#include <fmt/format.h>
#include <map>
#include <memory>
#include <sstream>
#include <string.h>
#include <string>
#include <utility>
#include <vector>

void SimulationManager::CollectMeasures()
{
    results.Collect(os->GetStation("CPU")->get());
    results.Collect(os->GetStation("IO1")->get());
    results.Collect(os->GetStation("IO2")->get());
    results.Collect(os->GetStation("SWAP_IN")->get());

    auto activeTime = [this](std::string name) {
        auto s = os->GetStation(name).value();
        //  return results._acc[name][StationStats::meanwait].mean() *
        //       ((double)s->completions() / os->GetStation("delay_station").value()->completions());

        return results._acc[name][StationStats::meancustomer].R();
    };
    results._activeTime((activeTime("CPU") + activeTime("IO1") + activeTime("IO2")) /
                        results._acc["SWAP_IN"][StationStats::throughput].R());
}

void SimulationManager::SetupScenario(std::string name)
{
    BaseScenario *s = nullptr;
    for (auto rs : _scenarios)
    {
        if (rs->name == name)
        {
            s = rs;
        }
    }
    if (s == nullptr)
    {
        logger.Exception("Cannot find scenario with name {}", name);
        return;
    }
    _currScenario = s;
    regPoint->Reset();
    HReset();
    s->Setup(this);
    regPoint->AddAction([this](RegenerationPoint *point) {
        // os->Sync();
        CollectMeasures();
        point->scheduler->Reset();
    });
    regPoint->scheduler = os.get();
    regPoint->simulator = os.get();
    results.tgt.WithRegPoint(regPoint.get());
    results.tgt.ConnectEntrance(os->GetStation("SWAP_IN").value().get(), false);
    results.tgt.ConnectLeave(os->GetStation("SWAP_OUT").value().get(), true);
}

void SimulationManager::HReset()
{
    os = std::unique_ptr<OS>(new OS());
    shell->SetControllers(os.get(), os.get());
    results.Reset();
}

SimulationManager::SimulationManager()
{
    os = std::unique_ptr<OS>(new OS());
    regPoint = std::unique_ptr<RegenerationPoint>(new RegenerationPoint(os.get(), os.get()));
}

void SimulationManager::SetupShell(SimulationShell *shell)
{
    this->shell = shell;
    SetupScenario("Default");

    shell->AddCommand("hreset", [this](SimulationShell *shell, auto ctx) mutable { HReset(); });
    shell->AddCommand("exit", [](auto s, auto c) { exit(0); });
    shell->AddCommand("regstats", [this](SimulationShell *s, auto c) {
        logger.Information("Regeneration point -> Hitted:{}, Called:{}", regPoint->hitted(), regPoint->called());
    });
    SystemParameters::Parameters().AddControlCommands(shell);

    shell->AddCommand("lstats", [this](auto s, auto c) {
        char buffer[32]{};
        std::istringstream read{c};
        read >> buffer;
        if (strlen(buffer) == 0)
        {
            for (auto s : os->GetStations())
            {
                os->Sync();
                logger.Result("S:{},B:{},O:{},A:{},C:{},N:{},W:{},MAXN:{},AN:{},AS:{}", s->Name(), s->busyTime(),
                              s->observation(), s->arrivals(), s->completions(), s->sysClients(), s->avg_waiting(),
                              s->max_sys_clients(), s->areaN(), s->areaS());
            }
        }
        else
        {
            std::string statName{buffer};
            auto ref = os->GetStation(statName);
            if (!ref.has_value())
            {
                logger.Exception("Station : {} not found", statName);
                return;
            }
            auto stat = ref.value();
            logger.Result("Station:{} \n Arrivals:{}\n Completions:{} \n Throughput:{} \n Utilization:{} \n "
                          "Avg_serviceTime:{} \n "
                          "Avg_Delay:{}\n Avg_interarrival:{} \n Avg_waiting:{} \n ",
                          stat->Name(), stat->arrivals(), stat->completions(), stat->throughput(), stat->utilization(),
                          stat->avg_serviceTime(), stat->avg_delay(), stat->avg_interArrival(), stat->avg_waiting());
        }
    });
    shell->AddCommand("lqueue", [this](auto s, auto c) {
        logger.Result("Scheduler Queue:{}", os->EventQueue());
        for (auto s : os->GetStations())
        {
            auto p = std::dynamic_pointer_cast<IQueueHolder>(s);
            if (p != nullptr)
            {
                logger.Result("Queue {}: {}", s->Name(), p->GetEventList());
            }
        }
    });
    shell->AddCommand("scenario", [this](SimulationShell *shell, const char *ctx) {
        char buffer[64]{};
        std::stringstream read(ctx);
        read >> buffer;
        if (strcmp(buffer, "list") == 0)
        {
            for (auto s : _scenarios)
            {
                fmt::println("Scenario: {}", s->name);
            }
        }
        else
        {
            std::string sc{buffer};
            SetupScenario(sc);
            fmt::println("Setup for scenario:{} completed", sc);
        }
    });

    shell->AddCommand("nr", [this](SimulationShell *shell, auto ctx) {
        char buffer[12]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        int m = 1;
        if (strlen(buffer) > 0)
        {
            m = atoi(buffer);
        }
        if (!stream.eof())
        {
            stream >> buffer;
            int log = atoi(buffer);
            if (log)
            {
                CollectSamples(m, true);
                return;
            }
        }
        CollectSamples(m);
    });

    auto l = [this](SimulationShell *shell, const char *context, bool arrival) {
        char buffer[36]{};
        std::stringstream read{context};
        read >> buffer;
        if (strlen(buffer) == 0)
            shell->Log()->Exception("Must select a station to let the event arrive");
        std::string stat{buffer};
        memset(buffer, 0, sizeof(buffer));
        read >> buffer;
        bool end = false;
        if (strlen(buffer) == 0)
        {
            // first event to arrive
            if (arrival)
                os->GetStation(stat).value()->OnArrivalOnce([&end](auto s, auto e) { end = true; });
            else
                os->GetStation(stat).value()->OnDepartureOnce([&end](auto s, auto e) { end = true; });
        }
        else
        {
            std::string capName{buffer};
            if (arrival)
                os->GetStation(stat).value()->OnArrivalOnce([&end, capName](auto s, Event &e) {
                    if (e.Name == capName)
                    {
                        end = true;
                    }
                });
            else
                os->GetStation(stat).value()->OnDepartureOnce([&end, capName](auto s, Event &e) {
                    if (e.Name == capName)
                    {
                        end = true;
                    }
                });
        }
        while (!end)
        {
            os->Execute();
        }
        logger.Information("{} of event detected", arrival ? "Arrival" : "Departure");
    };

    shell->AddCommand("na", [this, l](SimulationShell *shell, const char *context) { l(shell, context, true); });
    shell->AddCommand("nd", [l](auto s, auto ctx) { l(s, ctx, false); });
    shell->AddCommand("ns", [this](SimulationShell *shell, const char *ctx) {
        regPoint->SetRules(false);
        std::stringstream stream{ctx};
        char buffer[32]{};
        stream >> buffer;
        int m = atoi(buffer);
        SearchStates(m);
        regPoint->SetRules(true);
    });
    shell->AddCommand("nn", [this](SimulationShell *shell, const char *ctx) {
        char buffer[12]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        int seed = 123456789;
        int m = 1;
        int log = 0;
        if (strlen(buffer) > 0)
        {
            seed = atoi(buffer);
        }
        else
        {
            fmt::println("Need a seed parameter");
            return;
        }
        if (!stream.eof())
        {
            stream >> buffer;
            m = atoi(buffer);
        }
        else
        {
            fmt::println("Need an iteration parameter");
            return;
        }
        if (!stream.eof())
        {
            stream >> buffer;
            log = atoi(buffer);
        }
        for (int i = 0; i < m; i++)
        {
            os->Initialize();
            CollectSamples(-1, log);
            results.LogResult();
            results.CollectResult(seed);
            seed++;
            RandomStream::Global().PlantSeeds(seed);
            shell->Log()->Result("Results for seed {}", seed);
            results.LogSimResults();
            SetupScenario(_currScenario->name);
        }
    });
    results.AddShellCommands(shell);
};

void SimulationManager::CollectSamples(int samples, bool logMeasures, bool logActualState)
{
    auto p = [this, logMeasures, logActualState](int i) {
        bool end = false;
        regPoint->AddOneTimeAction([&end, logMeasures, logActualState, this](auto regPoint) { end = true; });
        while (!end)
        {
            os->Execute();
        }
        logger.Information("Collected {} samples (RegPoint end)", i);
        if (logMeasures)
        {
            for (auto tg : results._precisionTargets)
            {
                results.LogResult(tg);
            }
        }
        if (logActualState)
        {
            auto print = [](BaseStation *s) {
                fmt::println("Station:{},N:{},A:{},C:{}", s->Name(), s->sysClients(), s->arrivals(), s->completions());
            };

            print(os->GetStation("CPU").value().get());
            print(os->GetStation("IO1").value().get());
            print(os->GetStation("IO2").value().get());
            print(os->GetStation("SWAP_IN").value().get());
            print(os->GetStation(0).value().get());
        }
    };
    if (samples == -1)
    {
        int i = 0;
        while (results.tgt._mean.confidence().precision() > 0.05 || results.tgt._mean.Count() < 40)
        {
            p(i);
            i++;
        }
    }
    for (int i = 0; i < samples; i++)
    {
        p(i);
    }
}

void SimulationManager::SearchStates(int iterations, bool logActualState)
{
    struct State
    {
        int N_delay = 0;
        int N_cpu = 0;
        int N_io1 = 0;
        int N_io2 = 0;
        int N_swap = 0;
        bool operator==(const State &state) const
        {
            return N_delay == state.N_delay && N_cpu == state.N_cpu && N_io1 == state.N_io1 && N_io2 == state.N_io2 &&
                   N_swap == state.N_swap;
        }
    };

    std::vector<std::pair<State, int>> hits{};
    bool end = false;
    auto execFnc = [this, &end, &hits]() {
        regPoint->AddOneTimeAction([&end](auto rs) { end = true; });
        while (!end)
        {
            os->Execute();
        }
        end = false;
        State s = {
            .N_delay = os->GetStation(0).value()->sysClients(),
            .N_cpu = os->GetStation("CPU").value()->sysClients(),
            .N_io1 = os->GetStation("IO1").value()->sysClients(),
            .N_io2 = os->GetStation("IO2").value()->sysClients(),
            .N_swap = os->GetStation("SWAP_IN").value()->sysClients(),
        };
        for (int i = 0; i < hits.size(); i++)
        {
            if (hits[i].first == s)
            {
                hits[i].second += 1;
                return;
            }
        }
        hits.push_back({s, 1});
    };
    auto printarray = [&hits]() {
        for (auto e : hits)
        {
            fmt::println("NDelay:{}, NSwap:{}, NCPU:{}, NIO1:{}, NIO2:{}, hits:{}", e.first.N_delay, e.first.N_swap,
                         e.first.N_cpu, e.first.N_io1, e.first.N_io2, e.second);
        }
    };
    for (int i = 0; i < iterations; i++)
    {
        execFnc();
    }
    std::sort(hits.begin(), hits.end(), [](auto s1, auto s2) { return s1.second > s2.second; });
    printarray();
}
