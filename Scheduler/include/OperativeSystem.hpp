#pragma once
#include "CPU.hpp"
#include "DataCollector.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "IOStation.hpp"
#include "ISimulator.hpp"
#include "ReserveStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "SwapIn.hpp"
#include "SwapOut.hpp"
#include "rngs.hpp"
#include <map>
#include <memory>
#include <vector>

class OS : public ISimulator, public Scheduler
{
  private:
    bool _end = false;
    VariableStream _interArrival;

  protected:
    void ProcessProbe(Event &evt) override;

  public:
    void Execute() override;
    void Reset() override;
    OS();
    void Initialize() override;

    virtual void ProcessArrival(Event &evt) override;
};