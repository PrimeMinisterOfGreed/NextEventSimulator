#pragma once
#include "Collections/LinkedList.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <iostream>

using EventList = DoubleLinkedList<Event>;

class NESssq : public Scheduler, public ISimulator
{
  protected:
    bool _end = false;
    VariableStream _serviceTimes;
    VariableStream _interArrivals;

  public:
    void Initialize() override;
    void Execute() override;
    void Reset() override;
    NESssq();
};
