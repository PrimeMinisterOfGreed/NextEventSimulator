#pragma once
#include "DataCollector.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include <vector>

class Station
{
  protected:
    std::string _name;
    int _stationIndex{};
    int _arrivals{};
    int _completions{};
    int _sysClients{};
    int _maxClients{};
    double _busyTime{};
    double _observationPeriod{};
    double _lastArrival{};
    double _areaN{};
    double _areaS{};
    double _oldclock{};
    double _clock{};
    DataCollector collector;
    virtual void ProcessArrival(Event &evt);
    virtual void ProcessDeparture(Event &evt);
    virtual void ProcessEnd(Event &evt);
    virtual void ProcessProbe(Event &evt);
    virtual void ProcessMaintenance(Event &evt);
    ILogEngine *_logger;

  public:
    void Process(Event &event);
    virtual void Initialize();
    void Update();
    DataCollector Data();
    virtual void Reset();
    Station(std::string name, int station);
    std::string Name() const
    {
        return _name;
    }
    int stationIndex() const
    {
        return _stationIndex;
    }

    int arrivals() const
    {
        return _arrivals;
    }

    int completions() const
    {
        return _completions;
    }

    int sysClients() const
    {
        return _sysClients;
    }
};

class DelayStation
{
  protected:
    std::string _name;
    double _thinkTime;
    int _numClients;

  public:
    Event &Process();
    DelayStation(double thinkTime, int numClients);
};