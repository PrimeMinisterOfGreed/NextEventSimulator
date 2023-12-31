#pragma once
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Scheduler.hpp"

class OS : public ISimulator, public Scheduler
{
  private:
    bool _end = false;

    const int _workstations = 20;

  protected:
    void ProcessProbe(Event &evt) override;

  public:
    void Execute() override;
    void Reset() override;
    OS();
    void Initialize() override;

    virtual void ProcessArrival(Event &evt) override;
};