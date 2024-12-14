/**
 * @file TaggedCustomer.hpp
 * @author matteo.ielacqua
 * @brief implementazione del tagged customer con relativi accumulatori
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Event.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include "RegenerationPoint.hpp"
#include <functional>
#include <map>
#include <optional>
#include <utility>
struct TaggedCustomer
{   
    // accumulatore della media totale dell'active time
    CovariatedMeasure _mean{"ActiveTime", "ms"};
    // accumulatore di misure entro il ciclo rigenerativo
    Accumulator<> _acc{"regTime", "ms"};
    std::optional<std::function<void(Event &)>> _onEntrance;
    std::optional<std::function<void(Event &)>> _onLeave;
    // cliente target individuato
    std::string target_client = "";
    TaggedCustomer()
    {
    }

    // connette il tagged customer all'evento di innesco del ciclo di rigeneramento 
    TaggedCustomer &WithRegPoint(RegenerationPoint *regPoint)
    {
        regPoint->AddAction([this](RegenerationPoint *pt) { CompleteRegCycle(); });
        return *this;
    }

    double time = 0;
    /**
     * @brief si connette a uno specifico evento di arrivo/partenza
     * di una stazione, che verrà considerato come entrata di un cliente nel sottosistema
     * di cui vogliamo raccogliere l'active time
     * @param station 
     * @param arrival 
     */
    void ConnectEntrance(BaseStation *station, bool arrival = false);
    /**
     * @brief funzione duale alla ConnectEntrance 
     * 
     * @param station 
     * @param arrival 
     */
    void ConnectLeave(BaseStation *station, bool arrival = false);
    /**
     * @brief completa un osservazione sommando tutti i tempi di passaggio
     * e mettendoli dentro un'accumulatore CovariatedMeasure usando il conto dell'accumulatore "temporaneo"
     * come intervallo di tempo per la stima dell'intervallo di confidenza
     */
    void CompleteRegCycle();
    // resetta i contatori
    void CompleteSimulation();

    template <typename F>
        requires(has_return_value<F, void, Event &>)
    void OnEntrance(F &&fnc)
    {
        _onEntrance = fnc;
    }

    template <typename F>
        requires(has_return_value<F, void, Event &>)
    void OnLeave(F &&fnc)
    {
        _onLeave = fnc;
    }
};