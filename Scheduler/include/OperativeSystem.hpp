#pragma once
#include "ISimulator.hpp"
#include "CPU.hpp"
#include "SwapIn.hpp"
#include "IOStation.hpp"
#include "ReserveStation.hpp"


class OS : public ISimulatorEsembler, public IScheduler
{
private:
    bool _end = false;
    double _interArrivalLambda;

    Cpu* _cpu;
    ReserveStation * _reserveStation;
    IOStation * _io1;
    IOStation * _io2;
    EventList & _eventQueue = *new EventList();
    double _msecThinkTime;
    void RouteToStation(Event * evt);
    NegExpVariable * _generator;
protected:
    void Initialize();
  public:
    void Execute() override;
    void Report() override;
    void Reset() override;
    OS();
    void Schedule(Event *event) override;
};