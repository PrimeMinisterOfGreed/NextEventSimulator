#pragma once
#include "EventHandler.hpp"
#include "ISimulator.hpp"
#include "CPU.hpp"
#include "Station.hpp"
#include "SwapIn.hpp"
#include "IOStation.hpp"
#include "ReserveStation.hpp"
#include "SwapOut.hpp"
#include "Statistics.hpp"
#include "rngs.hpp"
#include <map>
#include <memory>
class OS : public ISimulatorEsembler, public IScheduler, public Station
{
private:
    bool _end = false;
    std::vector<Station*> _stations;
    Cpu _cpu;
    ReserveStation _reserveStation;
    IOStation _io1;
    IOStation _io2;
    SwapIn _swapIn;
    SwapOut _swapOut;
    EventList _eventQueue{};
    void RouteToStation(Event * evt);
    std::unique_ptr<VariableStream> _nextArrival;  
protected:
    void Initialize() override;
  public:
    void Execute() override;
    void Report() override;
    void Reset() override;
    EventHandler<StationStatistic> OnEventProcess;
    EventHandler<StationStatistic> OnProcessFinished;
    OS(ILogEngine * logger, double cpuSliceTime, int multiProgrammingDegree);
    void Schedule(Event *event) override;
};