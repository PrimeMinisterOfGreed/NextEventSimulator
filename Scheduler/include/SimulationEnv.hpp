#pragma once
#include "DataWriter.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "SimulationResult.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "Strategies/TaggedCustomer.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

void SetupEnvironment();
struct BaseScenario;
struct SimulationManager
{

    std::vector<BaseScenario *> _scenarios{};
    BaseScenario* _currScenario;
    std::vector<std::function<void()>> _collectFunctions{};
    std::unique_ptr<OS> os;
    std::unique_ptr<RegenerationPoint> regPoint;
    SimulationShell *shell;
    TraceSource logger{"SIMManager", 4};
    SimulationResult results{};
    bool hot = false;
    SimulationManager();

    static SimulationManager &Instance()
    {
        static SimulationManager manager{};
        return manager;
    }

    void AddScenario(BaseScenario *s)
    {
        _scenarios.push_back(s);
    }

    void RemoveScenario(BaseScenario *s)
    {
        std::erase_if(_scenarios, [s](auto s1) { return s == s1; });
    }

    void SetupShell(SimulationShell *shell);
    void HReset();
    void CollectMeasures();

  private:
    void SetupScenario(std::string name);
    void SetupEnvironment();
    void CollectSamples(int samples, bool logMeasures = false, bool logActualState = false);
    void SearchStates(int iterations, bool logActualState = false);
};

struct BaseScenario
{
    virtual void Setup(SimulationManager *manager) = 0;
    std::string name;
    BaseScenario(std::string name) : name(name)
    {
        SimulationManager::Instance().AddScenario(this);
    }
    ~BaseScenario()
    {
        SimulationManager::Instance().RemoveScenario(this);
    }
};
