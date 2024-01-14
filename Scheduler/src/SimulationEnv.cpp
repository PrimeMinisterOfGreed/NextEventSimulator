#include "SimulationEnv.hpp"
#include "Core.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <fmt/core.h>
#include <memory>
#include <sstream>

void SimulationManager::AddStationToCollectibles(std::string name)
{
    using namespace fmt;
    auto t = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_throughput", name), "j/s"});
    auto u = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_utilization", name), ""});
    auto n = _acc.size();
    _acc.push_back(Accumulator<>{format("{}_meanclients", name), ""});
    _collectFunctions.push_back([t, u, n, name, this] {
        auto station = os->GetStation(name).value();
        station->Update();
        _acc[t](station->throughput());
        _acc[u](station->utilization());
        _acc[n](station->mean_customer_system());
    });
}

void SimulationManager::CollectMeasures()
{
    for (auto f : _collectFunctions)
    {
        f();
    }
}

void SimulationManager::SetupScenario(std::string name)
{
    regPoint->Reset();
    HReset();
    for (auto s : _scenarios)
    {
        if (name == s->name)
        {
            s->Setup(this);
        }
    }
    regPoint->AddAction([this](RegenerationPoint *point) {
        CollectMeasures();
        point->scheduler->Reset();
    });
    regPoint->scheduler = os.get();
    regPoint->simulator = os.get();
    tgt.WithRegPoint(regPoint.get());
    tgt.ConnectEntrance(os->GetStation("SWAP_IN").value().get(), false);
    tgt.ConnectLeave(os->GetStation("SWAP_OUT").value().get(), true);
    _acc.clear();
    AddStationToCollectibles("CPU");
    AddStationToCollectibles("IO1");
    AddStationToCollectibles("IO2");
    AddStationToCollectibles("SWAP_IN");
    AddStationToCollectibles("SWAP_OUT");
    AddStationToCollectibles("RESERVE_STATION");
}

void SimulationManager::HReset()
{
    os = std::unique_ptr<OS>(new OS());
    shell->SetControllers(os.get(), os.get());
    for (auto m : _acc)
    {
        m.Reset();
    }
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
    shell->AddCommand("lmeasures", [this](SimulationShell *shell, auto c) {
        for (auto a : _acc)
        {
            shell->Log()->Result("{}", a);
        }
    });
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
                s->Update();
                logger.Result("S:{},B:{},O:{},A:{},S:{},N:{},W:{}", s->Name(), s->busyTime(), s->observation(),
                              s->arrivals(), s->completions(), s->sysClients(), s->avg_waiting());
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
    tgt.AddShellCommands(shell);
    shell->AddCommand("lqueue", [this](auto s, auto c) { logger.Result("Scheduler Queue:{}", os->EventQueue()); });
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
    shell->AddCommand("activetimes", [this](auto s, auto ctx) {
        double sum = os->GetStation("CPU").value()->avg_waiting() + os->GetStation("IO1").value()->avg_waiting() +
                     os->GetStation("IO2").value()->avg_waiting();
        logger.Result("Sum of active times {}", sum);
    });

    shell->AddCommand("nr", [this](SimulationShell *shell, auto ctx) {
        char buffer[12]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        int m = 1;
        if (strlen(buffer) > 0)
            m = atoi(buffer);
        for (int i = 0; i < m; i++)
        {
            bool end = false;
            regPoint->AddOneTimeAction([&end](auto regPoint) { end = true; });
            while (!end)
            {
                os->Execute();
            }
            logger.Information("End of {} rengeneration cycle", i);
        }
    });
};
