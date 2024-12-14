/**
 * @file CPU.cpp
 * @author matteo ielacqua
 * @brief Definizione del funzionamento della stazione di CPU, istanza sostanzialmente di una stazione generica
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "CPU.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "Usings.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <vector>


Cpu::Cpu(IScheduler *scheduler) : Station("CPU", Stations::CPU), _scheduler(scheduler)
{
}

/**
 * @brief Processa un evento di arrivo
 * 
 * @param evt 
 */
void Cpu::ProcessArrival(Event &evt)
{

    Station::ProcessArrival(evt);
    evt.ServiceTime = Burst();
    _logger.Debug("Assigned burst time {}", evt.ServiceTime);
    if (_eventUnderProcess.has_value())
    {
        _eventList.Enqueue(evt);
        _logger.Debug("CPU is busy, enqueue process {}, queue size {}", evt, _eventList.Count());
        return;
    }
    Manage(evt);
}
/**
 * @brief Gestore degli eventi per la cpu, contiene anche gli stream objects per la generazione
 * di istanze delle variabili aleatorie.
 * 
 * @param evt 
 */
void Cpu::Manage(Event &evt)
{
    static VariableStream sliceStream{99,
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().cpuQuantum); }};

    auto quantum = SystemParameters::Parameters().slicemode == SystemParameters::FIXED
                       ? SystemParameters::Parameters().cpuQuantum
                       : sliceStream();
                           evt.Type = DEPARTURE;
    if(SystemParameters::Parameters().burstMode == SystemParameters::FIXED){
        evt.ServiceTime = Burst();
    }
    auto slice = quantum < evt.ServiceTime ? quantum : evt.ServiceTime;
    evt.ServiceTime -= slice;
    evt.OccurTime = _clock + slice;
    _scheduler->Schedule(evt);
    _eventUnderProcess = evt;
}

/**
 * @brief Processa un evento di partenza
 * 
 * @param evt 
 */
void Cpu::ProcessDeparture(Event &evt)
{

    static Router router(4, SystemParameters::Parameters().cpuChoice, {IO_1, IO_2, SWAP_OUT, CPU});
    // process has finished
    core_assert(_eventUnderProcess.has_value(), "Event {} in departure but no event under process", evt);
    core_assert(evt == _eventUnderProcess.value(), "Event {} scheduled for departure but other event in process {}",
                evt, _eventUnderProcess.value());
    _eventUnderProcess.reset();
    if (evt.ServiceTime == 0)
    {
        evt.Type = ARRIVAL;
        evt.Station = router();
        evt.SubType = 0;
        _scheduler->Schedule(evt);
        Station::ProcessDeparture(evt);
    }
    else
    {
        _eventList.Enqueue(evt);
        _completions++;
    }
    if (_eventList.Count() > 0)
    {
        auto newEvt = _eventList.Dequeue();
        newEvt.OccurTime = _clock;
        Manage(newEvt);
        _logger.Debug("Managing {} from ready queue, remainig SV time {}", newEvt, newEvt.ServiceTime);
    }
}

/**
 * @brief Genera un burst con distribuzione iper esponenziale per il processo 
 * che sta attraversando la fase di processamento, questa funzione viene usata 
 * solo se il timeslice (burstmode) è in modalità fissa (FIXED)
 * @return double 
 */
double Cpu::Burst()
{
    static CompositionStream hyperExp{3,
                                      {SystemParameters::Parameters().alpha, SystemParameters::Parameters().beta},
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().u1); },
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().u2); }};

    return hyperExp();
}
