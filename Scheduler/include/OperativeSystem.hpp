#pragma once
#include "CPU.hpp"
#include "DataCollector.hpp"
#include "EventHandler.hpp"
#include "FunctionPointer.hpp"
#include "IOStation.hpp"
#include "ISimulator.hpp"
#include "ReserveStation.hpp"
#include "Station.hpp"
#include "SwapIn.hpp"
#include "SwapOut.hpp"
#include "rngs.hpp"
#include <map>
#include <memory>
#include <vector>
class OS : public ISimulatorEsembler, public IScheduler, public Station
{
  private:
    bool _end = false;
    std::vector<Station *> _stations;
    Cpu _cpu;
    ReserveStation _reserveStation;
    IOStation _io1;
    IOStation _io2;
    SwapIn _swapIn;
    SwapOut _swapOut;
    EventList _eventQueue{};
    void RouteToStation(Event &evt);
    std::unique_ptr<VariableStream> _nextArrival;

  protected:
    void Initialize() override;

  public:
    void Execute() override;
    void Reset() override;
    FunctionPointer<void, std::vector<Station *>> OnEventProcess;
    FunctionPointer<void, std::vector<Station *>> OnProcessFinished;
    OS(double cpuSliceTime, int multiProgrammingDegree);
    void Schedule(Event event) override;
};