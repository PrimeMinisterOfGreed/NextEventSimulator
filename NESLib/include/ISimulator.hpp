#pragma once
#include "DataCollector.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "Station.hpp"
#include "Usings.hpp"
#include <memory>
#include <optional>
#include <vector>
class ISimulator
{
  public:
    virtual void Execute() = 0;
};

class IScheduler
{
  public:
    virtual void Schedule(Event event) = 0;
    virtual void Reset() = 0;
    virtual void Sync() = 0;
    virtual std::optional<sptr<Station>> GetStation(int index) = 0;
    virtual std::optional<sptr<Station>> GetStation(std::string name) = 0;
};
