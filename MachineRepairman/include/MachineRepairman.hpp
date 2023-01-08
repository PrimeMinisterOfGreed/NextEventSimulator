#pragma once
#include "ISimulator.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "Station.hpp"
#include "LinkedList.hpp"

class MachineRepairman : public ISimulator
{
  private:
    DoubleLinkedList<Event>& _eventList;
  public:
    void Execute() override;
    void Report() override;
    void Schedule(Event &evt);
    MachineRepairman();
};        