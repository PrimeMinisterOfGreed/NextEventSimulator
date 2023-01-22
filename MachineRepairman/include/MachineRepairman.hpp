#pragma once
#include "DataProvider.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <vector>
#include "LogEngine.hpp"
#include "Station.hpp"
#include "LinkedList.hpp"

class MachineRepairman : public ISimulatorEsembler, public IScheduler
{
  private:
    DoubleLinkedList<Event> &_eventList = *new DoubleLinkedList<Event>();
    Station *_repairStation;
    Station * _machineStation;
    NegExpVariable * _interArrivalVariable;
    NegExpVariable * _serviceVariable;
    ILogEngine *_logger;
    double _clock = 0.0;
    double _endTime = 0.0;
    bool _autoEnd = false;
    void Initialize();
  public:
    void Execute() override;
    void Report() override;
    std::vector<StationStatistic> GetStats() const;
    void Schedule(Event *event) override;
    void Reset() override;
    MachineRepairman(ILogEngine* logger, IGenerator * stream, double serviceLambda , double interarrivalLambda , double endTime = 0.0);
};        