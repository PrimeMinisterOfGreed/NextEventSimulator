#pragma once
#include "ISimulator.hpp"
#include "CPU.hpp"
#include "SwapIn.hpp"
#include "IOStation.hpp"
#include "ReserveStation.hpp"
#include "SwapOut.hpp"

class OS : public ISimulatorEsembler, public IScheduler
{
private:
    bool _end = false;
    std::vector<Station*>* _stations;
    Cpu* _cpu;
    ReserveStation * _reserveStation;
    IOStation * _io1;
    IOStation * _io2;
    SwapIn * _swapIn;
    SwapOut * _swapOut;
    EventList & _eventQueue = *new EventList();
    void RouteToStation(Event * evt);
    NegExpVariable * _nextArrival;
protected:
    void Initialize();
  public:
    void Execute() override;
    void Report() override;
    void Reset() override;
    OS(ILogEngine * logger, double cpuSliceTime, int multiProgrammingDegree);
    void Schedule(Event *event) override;
};