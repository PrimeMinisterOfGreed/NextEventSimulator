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
#include <vector>

void SimulationManager::CollectMeasures()
{
    results.Collect(os->GetStation("CPU")->get());
    results.Collect(os->GetStation("IO1")->get());
    results.Collect(os->GetStation("IO2")->get());
    results.Collect(os->GetStation("SWAP_IN")->get());
    results.Collect(os->GetStation("SWAP_OUT")->get());

    auto activeTime = [this](std::string name) {
        auto s = os->GetStation(name).value();
        return results._acc[name][StationStats::meanwait].mean() *
               ((double)s->completions() / os->GetStation("delay_station").value()->completions());
    };
    results._activeTime( activeTime("CPU") + activeTime("IO1") + activeTime("IO2"));
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
            m = atoi(buffer);
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
        char buffer[36]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        if (sizeof(buffer) == 0)
        {
            shell->Log()->Exception("Must specify a starter seed to init");
            return;
        }

        long seed = atol(buffer);
        memset(buffer, 0, sizeof(buffer));
        stream >> buffer;
        if (strlen(buffer) == 0)
        {
            shell->Log()->Exception("Must specify a number of simulation (-1 for use a seq stopping rule)");
            return;
        }

        int nsim = atoi(buffer);
        memset(buffer, 0, sizeof(buffer));

        stream >> buffer;
        if (strlen(buffer) == 0)
        {
            shell->Log()->Exception("Must specify a number of samples to collect");
            return;
        }
        int samples = atoi(buffer);
        for (int i = 0; i < nsim; i++)
        {
            RandomStream::Global().PlantSeeds(seed);
            CollectSamples(samples);
            results.tgt.CompleteSimulation();
            results.CollectResult(seed);
            results.Reset();
            shell->Log()->Debug("Perform antitetich evaluation");
            RandomStream::Global().SetAntitetich(true);
            CollectSamples(samples);
            results.tgt.CompleteSimulation();
            results.CollectResult(seed);
            results.Reset();
            RandomStream::Global().SetAntitetich(false);
            seed++;
            shell->Log()->Debug("End of simulation {}", i);
            shell->Log()->Result("{}", results._activeTime);
        }
    });
    results.AddShellCommands(shell);
};

void SimulationManager::CollectSamples(int samples)
{
    auto p = [this](int i) {
        bool end = false;
        regPoint->AddOneTimeAction([&end](auto regPoint) { end = true; });
        while (!end)
        {
            os->Execute();
        }
        logger.Information("Collected {} samples (RegPoint end)", i);
    };
    if (samples == -1)
    {
        int i = 0;
        while (!results.PrecisionReached() || i < 40)
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
