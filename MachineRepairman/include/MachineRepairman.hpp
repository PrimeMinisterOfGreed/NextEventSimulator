#pragma once
#include "DataProvider.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include "LogEngine.hpp"
#include "Station.hpp"
#include "LinkedList.hpp"

class MachineRepairman : public ISimulator, public IScheduler
{
  private:
    DoubleLinkedList<Event> &_eventList = *new DoubleLinkedList<Event>();
    Station *_repairStation;
    IDataProvider *_provider;
    ILogEngine *_logger;
    int _generated = 0;
    int _deleted = 0;
    double _clock = 0.0;
  public:
    void Execute() override;
    void Report() override;
    void Schedule(Event * event) override;
    MachineRepairman(ILogEngine* logger, IDataProvider * provider, double endTime);
};        