/**
 * @file SimulationEnv.cpp
 * @author matteo.ielacqua
 * @see SimulationEnv.hpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "SimulationEnv.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "SimulationResult.hpp"
#include "Station.hpp"
#include "RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include "Usings.hpp"
#include "rngs.hpp"
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <cstring>

#include <map>

#include <memory>
#include <sstream>
#include <string.h>
#include <string>
#include <utility>
#include <vector>


void SimulationManager::CollectMeasures()
{
    results.Collect(*os->GetStation("CPU")->get());
    results.Collect(*os->GetStation("IO1")->get());
    results.Collect(*os->GetStation("IO2")->get());
    results.Collect(*os->GetStation("SWAP_IN")->get());    
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
    results.Reset();
}

SimulationManager::SimulationManager()
{
    os = std::unique_ptr<OS>(new OS());
    regPoint = std::unique_ptr<RegenerationPoint>(new RegenerationPoint(os.get(), os.get()));
}



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
                fmt::print("Station:{},N:{},A:{},C:{}", s->Name(), s->sysClients(), s->arrivals(), s->completions());
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
            fmt::print("NDelay:{}, NReserve:{}, NSwap:{}, NCPU:{}, NIO1:{}, NIO2:{},NOUT:{}, hits:{}",
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
