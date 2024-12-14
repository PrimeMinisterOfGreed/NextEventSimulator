/**
 * @file ISimulator.hpp
 * @author instestazione per una generica interfaccia di un qualunque simulatore
 * che ci si aspetta possa essere inizializzato ed eseguito
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Event.hpp"
#include "Station.hpp"
#include "Usings.hpp"
#include <memory>
#include <optional>
#include <vector>
class ISimulator
{
  public:
    virtual void Execute() = 0;
    virtual void Initialize() = 0;
};

/**
 * @brief Uno scheduler è un contenitore di stazioni che può gestire e smistare i vari eventi 
 * in arrivo
 * 
 */
class IScheduler
{
  public:
    virtual void Schedule(Event event) = 0;
    virtual void Reset() = 0;
    virtual void Sync() = 0;
    virtual std::optional<sptr<Station>> GetStation(int index) = 0;
    virtual std::optional<sptr<Station>> GetStation(std::string name) = 0;
    virtual const std::vector<sptr<Station>> GetStations() const = 0;
    virtual double GetClock() = 0;
};

/**
 * @brief interfaccia per riconoscere i contentitori di eventi 
 * 
 */
class IQueueHolder
{
  protected:
  EventList _eventList;
  public:
  EventList& GetEventList(){return _eventList;}
};