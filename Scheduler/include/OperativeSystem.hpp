#pragma once
#include "CPU.hpp"
#include "DataCollector.hpp"
#include "EventHandler.hpp"
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

class OS : public ISimulator, public IScheduler, public Station
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
    void ProcessProbe(Event &evt) override;

  public:
    void Execute() override;
    void Reset() override;
    std::function<void(sptr<Station>)> OnEventProcess;
    std::function<void(sptr<Station>)> OnProcessFinished;
    OS();
    void Schedule(Event event) override;
};