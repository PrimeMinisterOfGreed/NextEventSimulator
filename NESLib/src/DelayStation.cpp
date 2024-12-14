/**
 * @file DelayStation.cpp
 * @author matteo.ielacqua
 * @brief implementazione della stazione di delay, istanza di una generica classe Station. 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Event.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "DelayStation.hpp"

/**
 * @brief inizializza la stazione accodando un numero di clienti 
 * che verranno scodati con un certo ritardo, il numero di clienti è 
 * parametro di sistema 
 * 
 */
void DelayStation::Initialize()
{
    Station::Initialize();
    _sysClients = _numclients();
    for (int i = 0; i < _numclients(); i++)
    {
        auto evt = Event(fmt::format("{}", Event::GeneratedNodes), DEPARTURE, _clock, _delayTime(), 0, 0, 0);
        _scheduler->Schedule(evt);
    }
}
/**
 * @brief Processa l'arrivo di un cliente
 * 
 * @param evt 
 */
void DelayStation::ProcessArrival(Event &evt)
{
    Station::ProcessArrival(evt);
    evt.OccurTime = _delayTime() + _clock;
    evt.Type = DEPARTURE;
    evt.Station = 0;
    _scheduler->Schedule(evt);
}

/**
 * @brief Processa la partenza di un cliente 
 * 
 * @param evt 
 */
void DelayStation::ProcessDeparture(Event &evt)
{
    Station::ProcessDeparture(evt);
}
