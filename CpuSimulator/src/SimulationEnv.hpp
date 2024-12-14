/**
 * @file SimulationEnv.hpp
 * @author matteo.ielacqua
 * @brief gestore dell'ambiente di simulazione
 * gestisce l'esecuzione e la raccolta di informazioni dalla simulazione 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include "LogEngine.hpp"
#include "OperativeSystem.hpp"
#include "SimulationResult.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include <cmath>
#include <cstdlib>

#include <functional>
#include <memory>
#include <vector>

void SetupEnvironment();
struct BaseScenario;
struct SimulationManager
{

    std::vector<BaseScenario *> _scenarios{};
    BaseScenario *_currScenario;
    std::vector<std::function<void()>> _collectFunctions{};
    std::unique_ptr<OS> os;
    std::unique_ptr<RegenerationPoint> regPoint;

    TraceSource logger{"SIMManager", 1};
    SimulationResult results{};
    SimulationManager();

    static SimulationManager *Instance()
    {
        static SimulationManager manager{};
        return &manager;
    }

    void AddScenario(BaseScenario *s)
    {
        _scenarios.push_back(s);
    }

    void RemoveScenario(BaseScenario *s)
    {
        std::erase_if(_scenarios, [s](auto s1) { return s == s1; });
    }
    // imposta uno scenario come "situazione di simulazione corrente"
    void SetupScenario(std::string name);
    // effettua un hard reset delle strutture contenute
    void HReset();
    /**
     * @brief itera su tutte le stazioni contenute chiedendo a SimulationResult
     * di collezionare le osservazioni che si desidera ottenere
     * 
     */
    void CollectMeasures();
    /**
     * @brief itera per un certo numero di volte fino a collezionare  
     * un certo numero di campioni
     * @param samples con n>0 colleziona i campioni selezionati, con n == -1
     * itera fino a collezionare abbastanza misure da far scendere la precisione sotto la soglia 
     * di precisione prestabilità dall'accumulatore dell'active time
     * @param logMeasures 
     * @param logActualState 
     */
    void CollectSamples(int samples, bool logMeasures = false, bool logActualState = false);
  private:
    // utility usata per avere un printout degli stati raggiunti quando un certo evento si innesca
    // non utilizzato direttamente dal progetto
    void SearchStates(int iterations, bool logActualState = false);


};

struct BaseScenario
{
    virtual void Setup(SimulationManager *manager) = 0;
    std::string name;
    BaseScenario(std::string name) : name(name)
    {
        SimulationManager::Instance()->AddScenario(this);
    }
    ~BaseScenario()
    {
        SimulationManager::Instance()->RemoveScenario(this);
    }
};
