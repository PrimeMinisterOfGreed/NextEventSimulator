#include "SimulationEnv.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "SystemParameters.hpp"

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
    params.cpuQuantum = 2.7;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        static int counter = 0;
        counter++;
        if (counter == 100)
        {
            manager->regPoint->Trigger();
            counter = 0;
        }
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
}

SCENARIO(Default) // first request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
    params.cpuQuantum = 2.7;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });

    auto &regPoint = manager->regPoint;
    // first CPU must have 0 clients because is hyperexp
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });
};

SCENARIO(Default_NOMPD)
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
    params.cpuQuantum = 2.7;
    params.multiProgrammingDegree = 1000;
    params.numclients = 20;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });

    auto &regPoint = manager->regPoint;
    // first CPU must have 0 clients because is hyperexp
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 4; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}

SCENARIO(NegExpCpu) // second request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::NEG_EXP;
    params.cpuQuantum = 2.7;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
    auto &regPoint = manager->regPoint;
    regPoint->AddRule(
        [](RegenerationPoint *reg) { return reg->scheduler->GetStation("CPU").value()->sysClients() == 0; });

    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO2").value()->sysClients() == 11; });
    regPoint->AddRule([](RegenerationPoint *r) { return r->scheduler->GetStation("IO1").value()->sysClients() == 1; });
}

SCENARIO(LTCpu) // third request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::HYPER_EXP;
    params.cpuQuantum = 2700;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
}

SCENARIO(NegExpLt) // last request
{
    auto &params = SystemParameters::Parameters();
    params.cpumode = SystemParameters::NEG_EXP;
    params.cpuQuantum = 2700;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
}