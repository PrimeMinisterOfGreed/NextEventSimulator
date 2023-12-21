#pragma once
#include "Event.hpp"
#include "EventHandler.hpp"
#include <memory>
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
};
