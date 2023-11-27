#pragma once
#include "DataCollector.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include <functional>
#include <optional>
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
    TraceSource _logger;

    std::optional<std::function<void(Station *)>> _onArrival;
    std::optional<std::function<void(Station *)>> _onDeparture;

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
    double observation() const
    {
        return _observationPeriod;
    }

    template <typename F> void OnDeparture(F &&fnc)
    {
        _onDeparture = fnc;
    }

    template <typename F> void OnArrival(F &&fnc)
    {
        _onArrival = fnc;
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