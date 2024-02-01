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

SCENARIO(Simplified)
{
    auto &params = SystemParameters::Parameters();
    params.numclients = 1;
    params.cpumode = SystemParameters::FIXED;
    auto &regPoint = manager->regPoint;
    params.cpuQuantum = 2.7;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    manager->os->GetStation("SWAP_OUT").value()->OnArrival([&regPoint](auto s, auto e) { regPoint->Trigger(); });
    regPoint->AddRule([](RegenerationPoint *r) {
        auto cpu = r->scheduler->GetStation("CPU").value();
        auto io1 = r->scheduler->GetStation("IO1").value();
        auto io2 = r->scheduler->GetStation("IO2").value();
        auto ac = [](BaseStation *s) { return s->arrivals() == s->completions() && s->completions() > 1; };
        return ac(cpu.get()) && ac(io1.get()) && ac(io2.get());
    });

    static int counter = 0;
    regPoint->AddRule([](RegenerationPoint *r) {
        if (counter >= 10)
        {
            counter = 0;
            return true;
        }
        counter++;
        return false;
    });
};

SCENARIO(Simplified_N20)
{
    auto &params = SystemParameters::Parameters();
    params.numclients = 20;
    params.cpumode = SystemParameters::FIXED;
    params.multiProgrammingDegree = 1000;
    params.cpuQuantum = 2.7;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });

    auto &regPoint = manager->regPoint;
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 11; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
    regPoint->AddRule(
        [](RegenerationPoint *r) { return r->scheduler->GetStation("SWAP_IN").value()->sysClients() == 1; });
}

SCENARIO(Default) // first request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
    params.cpuQuantum = 2.7;

    auto &regPoint = manager->regPoint;
    // first CPU must have 0 clients because is hyperexp

    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });
    /* regPoint->AddRule(
         [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
     regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 6; });
     regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
     */
    regPoint->AddAction([os = manager->shell->Scheduler()](auto r) {
        for (auto s : os->GetStations())
        {
            os->Sync();
            fmt::println("S:{},N:{},MN:{},A:{},C:{}", s->Name(), s->sysClients(), s->max_sys_clients(), s->arrivals(),
                         s->completions());
        }
    });
};

SCENARIO(Default_NOMPD)
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
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
    params.cpumode = SystemParameters::NEG_EXP;
    params.cpuQuantum = 2.7;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 6; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}

SCENARIO(LTCpu) // third request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
    params.cpuQuantum = 2700;

    auto &regPoint = manager->regPoint;
    manager->results.tgt.OnEntrance([&regPoint](auto e) { regPoint->Trigger(); });

    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 6; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}

SCENARIO(NegExpLt) // last request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::NEG_EXP;
    params.cpuQuantum = 2700;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
    auto &regPoint = manager->regPoint;
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 6; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}