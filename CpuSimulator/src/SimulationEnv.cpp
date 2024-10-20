#include "SimulationEnv.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
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

// region COMMANDS
void list_stats(OS *os, TraceSource &logger, const char *c)
{
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
}

void show_queue(OS *os, TraceSource &logger)
{
    logger.Result("Scheduler Queue:{}", os->EventQueue());
    for (auto s : os->GetStations())
    {
        auto p = std::dynamic_pointer_cast<IQueueHolder>(s);
        if (p != nullptr)
        {
            logger.Result("Queue {}: {}", s->Name(), p->GetEventList());
        }
    }
}

void group_cycles(const char *c)
{
    auto stream = std::stringstream(c);
    char buffer[16]{};
    stream >> buffer;
    int group = atoi(buffer);
    SystemParameters::Parameters().groupRegCycle = group > 0 ? group : 1;
}

void attach(const char *context, SimulationShell *shell, bool arrival, OS *os, TraceSource &logger)
{
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
}

void SimulationManager::search_states(const char *ctx)
{
    regPoint->SetRules(false);
    std::stringstream stream{ctx};
    char buffer[32]{};
    stream >> buffer;
    int m = atoi(buffer);
    SearchStates(m);
    regPoint->SetRules(true);
}

void SimulationManager::select_scenario(const char *ctx)
{
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
    }
}

void SimulationManager::perform_number_regeneration(const char *ctx)
{
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
}

// endregion COMMANDS

void SimulationManager::CollectMeasures()
{
    results.Collect(*os->GetStation("CPU")->get());
    results.Collect(*os->GetStation("IO1")->get());
    results.Collect(*os->GetStation("IO2")->get());
    results.Collect(*os->GetStation("SWAP_IN")->get());

    auto activeTime = [this](std::string name) {
        auto s = os->GetStation(name).value();
        //  return results._acc[name][StationStats::meanwait].mean() *
        //       ((double)s->completions() / os->GetStation("delay_station").value()->completions());

        return results._acc[name][StationStats::meancustomer].R();
    };
    results.CollectActiveTime((activeTime("CPU") + activeTime("IO1") + activeTime("IO2")) /
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
        os->Sync();
        CollectMeasures();
        point->scheduler->Reset();
        os->Sync();
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
        s->Log()->Information("Regeneration point -> Hitted:{}, Called:{}", regPoint->hitted(), regPoint->called());
    });
    SystemParameters::Parameters().AddControlCommands(shell);

    shell->AddCommand("lstats", [this](auto s, auto c) { list_stats(os.get(), logger, c); });
    shell->AddCommand("lqueue", [this](auto s, auto c) { show_queue(os.get(), logger); });
    shell->AddCommand("group", [](auto s, auto c) { group_cycles(c); });
    shell->AddCommand("scenario", [this](SimulationShell *shell, const char *ctx) { this->select_scenario(ctx); });
    shell->AddCommand("sreset", [&](auto... ns) { this->results.Reset(); });
    shell->AddCommand("nr", [this](SimulationShell *shell, auto ctx) { perform_number_regeneration(ctx); });

    shell->AddCommand(
        "na", [&](SimulationShell *shell, const char *context) { attach(context, shell, true, os.get(), logger); });
    shell->AddCommand("nd", [&](auto s, auto ctx) { attach(ctx, s, true, os.get(), logger); });
    shell->AddCommand("ns", [this](SimulationShell *shell, const char *ctx) { search_states(ctx); });
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
        if (logMeasures && !results.IsTransitoryPeriod())
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
        int N_reserve = 0;
        int N_cpu = 0;
        int N_io1 = 0;
        int N_io2 = 0;
        int N_swap = 0;
        int N_out = 0;
        bool operator==(const State &state) const
        {
            return N_delay == state.N_delay && N_cpu == state.N_cpu && N_io1 == state.N_io1 && N_io2 == state.N_io2 &&
                   N_swap == state.N_swap && N_out == state.N_out && N_reserve == state.N_reserve;
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
            .N_reserve = os->GetStation("RESERVE_STATION").value()->sysClients(),
            .N_cpu = os->GetStation("CPU").value()->sysClients(),
            .N_io1 = os->GetStation("IO1").value()->sysClients(),
            .N_io2 = os->GetStation("IO2").value()->sysClients(),
            .N_swap = os->GetStation("SWAP_IN").value()->sysClients(),
            .N_out = os->GetStation("SWAP_OUT").value()->sysClients(),
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
            fmt::println("NDelay:{}, NReserve:{}, NSwap:{}, NCPU:{}, NIO1:{}, NIO2:{},NOUT:{}, hits:{}",
                         e.first.N_delay, e.first.N_reserve, e.first.N_swap, e.first.N_cpu, e.first.N_io1,
                         e.first.N_io2, e.first.N_out, e.second);
        }
    };
    for (int i = 0; i < iterations; i++)
    {
        execFnc();
    }
    std::sort(hits.begin(), hits.end(), [](auto s1, auto s2) { return s1.second > s2.second; });
    printarray();
}
