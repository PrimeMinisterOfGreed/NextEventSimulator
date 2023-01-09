#pragma once
#include "ISimulator.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "LogEngine.hpp"
#include "Station.hpp"
#include "LinkedList.hpp"

class MachineRepairman : public ISimulator, public IScheduler
{
  private:
    DoubleLinkedList<Event> &_eventList;
    Station &_repairStation;
    Station &_deliveryStation;
  public:
    void Execute() override;
    void Report() override;
    void Schedule(Event &evt) override;
    MachineRepairman(ILogEngine* logger);
};        