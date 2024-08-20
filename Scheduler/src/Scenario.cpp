#include "SimulationEnv.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include <fmt/core.h>
#include <string>
#include <tuple>
#include <vector>

#define SCENARIO(name)                                                                                                 \
    struct _Scenario_##name : public BaseScenario                                                                      \
    {                                                                                                                  \
        virtual void Setup(SimulationManager *manager) override;                                                       \
        _Scenario_##name() : BaseScenario(#name)                                                                       \
        {                                                                                                              \
        }                                                                                                              \
    };                                                                                                                 \
    _Scenario_##name __##name{};                                                                                       \
    void _Scenario_##name::Setup(SimulationManager *manager)

void AddClientConditionRule(RegenerationPoint *reg, std::vector<std::pair<std::string, int>> rules)
{
    for (auto rule : rules)
    {
        reg->AddRule([rule](RegenerationPoint *pt) {
            return pt->scheduler->GetStation(rule.first).value()->sysClients() == rule.second;
        });
    }

    // add regroup rule
    reg->AddRule([](auto r) {
        static int iterations = 0;
        iterations++;
        if (iterations % SystemParameters::Parameters().groupRegCycle == 0)
        {
            iterations = 0;
            return true;
        }
        return false;
    });
}

SCENARIO(Simplified_N20)
{
    auto &params = SystemParameters::Parameters();
    params.numclients = 20;
    params.alpha = 0.5;
    params.beta = 0.5;
    params.u1 = 2.7;
    params.u2 = 2.7;
    params.multiProgrammingDegree = 1000;
    params.cpuQuantum = 2700;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    // NDelay:3, NReserve:0, NSwap:0, NCPU:0, NIO1:0, NIO2:16,NOUT:0, hits:24
    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 15}, {"delay_station", 4}, {"RESERVE_STATION", 0}});
}

SCENARIO(Default) // first request
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2.7;

    auto &regPoint = manager->regPoint;
    // first CPU must have 0 clients because is hyperexp
    auto os = manager->shell->Scheduler();
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 9}, {"delay_station", 2}, {"RESERVE_STATION", 8}});
};

SCENARIO(NegExpCpu) // second request
{
    auto &params = SystemParameters::Parameters();

    params.cpuQuantum = 2.7;
    params.alpha = 0.5;
    params.beta = 0.5;
    params.u1 = 27;
    params.u2 = 27;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 9}, {"delay_station", 3}, {"RESERVE_STATION", 7}});
}

SCENARIO(LTCpu) // third request
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2700;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 9}, {"delay_station", 3}, {"RESERVE_STATION", 7}});
}

SCENARIO(NegExpLt) // last request
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2700;
    params.alpha = 0.5;
    params.beta = 0.5;
    params.u1 = 27;
    params.u2 = 27;
    auto &regPoint = manager->regPoint;
    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 9}, {"delay_station", 3}, {"RESERVE_STATION", 7}});
}

SCENARIO(Markov_20)
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 3;
    params.multiProgrammingDegree = 1000;
    params.numclients = 20;
    params.u1 = 15;
    params.u2 = 75;
    params.burstMode = SystemParameters::FIXED;
    params.alpha = 0.8;
    params.averageSwapIn = 0;
    params.beta = 0.2;
    params.slicemode = SystemParameters::NEG_EXP;
    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    AddClientConditionRule(regPoint.get(),
                           {{"CPU", 0}, {"IO1", 0}, {"IO2", 16}, {"delay_station", 3}, {"RESERVE_STATION", 0}

                           });
}
