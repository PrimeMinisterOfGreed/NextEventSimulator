#include "SimulationEnv.hpp"

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
    params.cpuUseNegExp = true;
    params.cpuQuantum = 2.7;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
};

SCENARIO(Default)
{
    auto &params = SystemParameters::Parameters();
    params.cpuUseNegExp = false;
    params.cpuQuantum = 2.7;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
};

SCENARIO(NegExpCpu)
{
    auto &params = SystemParameters::Parameters();
    params.cpuUseNegExp = true;
    params.cpuQuantum = 2.7;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
}

SCENARIO(LTCpu)
{
    auto &params = SystemParameters::Parameters();
    params.cpuUseNegExp = false;
    params.cpuQuantum = 2700;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
}

SCENARIO(NegExpLt)
{
    auto &params = SystemParameters::Parameters();
    params.cpuUseNegExp = true;
    params.cpuQuantum = 2700;
    manager->os->GetStation("SWAP_OUT").value()->OnDeparture([manager](auto s, auto e) {
        manager->regPoint->Trigger();
    });
}