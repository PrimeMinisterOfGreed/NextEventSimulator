/**
 * @file RegenerationPoint.hpp
 * @author matteo.ielacqua
 * @brief classe che implementa il concetto di innesco del punto di rigeneramento
 * inteso come coppia (event,set di regole), nel simulatore il set di regole altro 
 * non sono altro che un insieme di asserzioni sulla lunghezza della coda es. CPU == 8,
 * l'insieme di questo set di regole viene usato come stato da raggiungere perchè il punto di 
 * rigeneramento si inneschi.
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "ISimulator.hpp"
#include <concepts>
#include <functional>
#include <vector>

/**
 * @brief concetto per istruire il compilatore su come  
 * filtrare i tipi che hanno un tipo di ritorno funzionale
 * @tparam F 
 * @tparam R 
 * @tparam Args 
 */
template <typename F, typename R, typename... Args>
concept has_return_value = requires(F a, Args... args) {
    {
        a(args...)
    } -> std::same_as<R>;
};

/**
 * @brief classe del punto di rigeneramento
 * 
 */
struct RegenerationPoint
{
  private:
    int _called = 0;
    int _hitted = 0;
    // quando disattivato basterà il solo evento a innescare il punto di rigeneramento
    // utile per capire quali stati si verificano e con quale probabilità a un determinato evento
    bool _rulesEnabled = true;
  public:
    IScheduler *scheduler;
    ISimulator *simulator;
    std::vector<std::function<bool(RegenerationPoint *)>> _rules;
    std::vector<std::function<void(RegenerationPoint *)>> _actions;
    std::vector<std::function<void(RegenerationPoint *)>> _onTimeActions;
    RegenerationPoint(IScheduler *sched, ISimulator *simulator);

    template <typename F>
        requires(has_return_value<F, bool, RegenerationPoint *>)
    RegenerationPoint& AddRule(F &&fnc)
    {
        _rules.push_back(fnc);
        return *this;
    }

    // aggiunge un azione da effettuare quando il punto di rigeneramento 
    // viene innescato, nel simulatore semplicemente raccoglie le osservazioni
    template <typename F>
        requires(has_return_value<F, void, RegenerationPoint *>)
    void AddAction(F &&fnc)
    {
        _actions.push_back(fnc);
    }

    // aggiunge un'azione che verrà eseguita una sola volta, la prima 
    // che il punto di rigeneramento viene innescato dopo averla invocata
    template <typename F>
        requires(has_return_value<F, void, RegenerationPoint *>)
    void AddOneTimeAction(F &&fnc)
    {
        _onTimeActions.push_back(fnc);
    }
    // controlla che tutte le regole siano rispettate e lancia le azioni
    void Trigger();
    void operator()()
    {
        Trigger();
    }

    int hitted() const
    {
        return _hitted;
    }
    int called() const
    {
        return _called;
    }
    void Reset()
    {
        _rules.clear();
        _actions.clear();
        _hitted = 0;
        _called = 0;
    }
    // disabilita o abilita il controllo delle regole
    void SetRules(bool enable){_rulesEnabled = enable;}
};