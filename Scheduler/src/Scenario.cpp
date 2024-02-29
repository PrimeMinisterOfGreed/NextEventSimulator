#include "SimulationEnv.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"
#include <fmt/core.h>

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

SCENARIO(Simplified_N20)
{
    auto &params = SystemParameters::Parameters();
    params.numclients = 20;
    params.alpha = 0.5;
    params.beta = 0.5;
    params.u1 = 27;
    params.u2 = 27;
    params.multiProgrammingDegree = 1000;
    params.cpuQuantum = 2700;
    params.u1 = 2.7; // becomes burst if negexp mode is used
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    // NDelay:5, NSwap:0, NCPU:0, NIO1:0, NIO2:14, hits:3
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 16; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation(0).value()->sysClients() == 3; });
}

SCENARIO(Default) // first request
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2.7;

    auto &regPoint = manager->regPoint;
    // first CPU must have 0 clients because is hyperexp
    // NDelay:2, NSwap:0, NCPU:0, NIO1:0, NIO2:9, hits:13
    auto os = manager->shell->Scheduler();
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 9; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation(0).value()->sysClients() == 2; });
};

SCENARIO(Default_NOMPD)
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2.7;
    params.multiProgrammingDegree = 1000;
    params.numclients = 20;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    // first CPU must have 0 clients because is hyperexp
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 14; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}

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
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 9; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation(0).value()->sysClients() == 2; });
}

SCENARIO(LTCpu) // third request
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 2700;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 9; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation(0).value()->sysClients() == 2; });
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

    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 9; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation(0).value()->sysClients() == 2; });
}

SCENARIO(Markov_20_NegExp)
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 3;
    params.multiProgrammingDegree = 1000;
    params.numclients = 20;
    // params.u1 = 15;
    // params.u2 = 75;
    // params.alpha = 0.8;
    params.averageSwapIn = 0;
    // params.beta = 0.2;
    params.u1 = 27;
    params.u2 = 27;
    params.alpha = 0.5;
    params.beta = 0.5;

    params.slicemode = SystemParameters::NEG_EXP;
    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    // first CPU must have 0 clients because is hyperexp
    regPoint
        ->AddRule([](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; })
        .AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; })
        .AddRule([](auto r) { return r->scheduler->GetStation(0).value()->sysClients() == 4; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 15; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
}

SCENARIO(Markov_20)
{
    auto &params = SystemParameters::Parameters();
    params.cpuQuantum = 3;
    params.multiProgrammingDegree = 1000;
    params.numclients = 20;
    params.u1 = 15;
    params.u2 = 75;
    params.alpha = 0.8;
    params.averageSwapIn = 0;
    params.beta = 0.2;

    params.slicemode = SystemParameters::NEG_EXP;
    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    // first CPU must have 0 clients because is hyperexp
    regPoint
        ->AddRule([](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; })
        .AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 0; })
        .AddRule([](auto r) { return r->scheduler->GetStation(0).value()->sysClients() == 4; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 15; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 0; });
}
