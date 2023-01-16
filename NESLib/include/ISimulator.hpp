#pragma once
#include "Event.hpp"

class ISimulator
{
  public:
    virtual void Execute() = 0;
    virtual void Report() = 0;
};

class ISimulatorEsembler : public ISimulator
{
  public:
    virtual void Reset() = 0;
};

class IScheduler
{
  public:
    virtual void Schedule(Event *event) = 0;
};