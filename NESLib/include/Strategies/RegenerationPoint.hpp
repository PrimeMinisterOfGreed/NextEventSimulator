#pragma once
#include "Collections/LinkedList.hpp"
#include "ISimulator.hpp"
#include "Measure.hpp"

class RegenerationPoint
{
  private:
    ISimulator *_simulator;
    IScheduler *_scheduler;
    int _minimumSize;
    int count = 0;

  public:
    RegenerationPoint(ISimulator *simulator, IScheduler *scheduler, int minimum_batch_size = 40);
    void TriggerPoint();
    void CollectStat();
    void Run();
    virtual bool ShouldStop() = 0;
};