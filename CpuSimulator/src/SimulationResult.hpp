/**
 * @file SimulationResult.hpp
 * @author matteo.ielacqua
 * @brief contenitore per le classi che rappresentano i risultati 
 * statistici della simulazione
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "Strategies/TaggedCustomer.hpp"

#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>



/**
 * @brief classe helper che colleziona in automatico 
 * le misure e le statistiche volute dalla stazione passata come parametro,
 * nel caso di questa simulazione sono le attese e i clienti medi verranno prese in considerazione
 */
struct StationStats
{
    std::string station_name{};
    CovariatedMeasure _acc[4];
    void Collect(const BaseStation& station);
    StationStats();
    enum MeasureType
    {
        throughput,
        utilization,
        meancustomer,
        meanwait,
        size
    };

    CovariatedMeasure &operator[](StationStats::MeasureType measure);
    void Reset();
    bool Ready();
};


std::string format_as(StationStats stats);

/**
 * @brief classe che raccoglie i vari risultati della simulazione
 * 
 */
struct SimulationResult
{
    static inline double requiredPrecision = 0.05;
    static inline double confidence = 0.90;
    /**
     * @brief mappa di nome_stazione-> accumulatori delle misure
     * 
     */
    std::map<std::string, StationStats> _acc;
    /**
     * @brief target di precisione, usati sostanzialmente per stampare 
     * le misure quando si usa SimulationManager::CollectSamples(n,true), poichè 
     * l'unico accumulatore di cui viene considerata la precisione come stopping rule 
     * è quello degli active time, volendo si può facilmente estendere per tutte le misure
     * contenute in questo vettore
     */
    std::vector<std::string> _precisionTargets = {"CPU", "IO1", "IO2", "ActiveTime"};

    // contenitore delle misure raccolte con il tagged customer
    TaggedCustomer tgt{};
    TraceSource _logger;

    SimulationResult();
    void Reset();
    // colleziona le misure da una certa stazione aggiungendole alla mappa
    void Collect(const BaseStation &station);
    // estensione per richiedere che tutti i target raggiungano la precisione voluta
    // inutilizzato per ora
    bool PrecisionReached();

    void AddPrecisionTarget(std::string name)
    {
        _precisionTargets.push_back(name);
    }

    // stampa le misure collezionate nello std output
    void LogResult(std::string name = "ALL");
};
