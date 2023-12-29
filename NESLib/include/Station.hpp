#pragma once
#include "DataCollector.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include <functional>
#include <optional>
#include <vector>

class BaseStation
{
  protected:
    virtual void ProcessArrival(Event &evt);
    virtual void ProcessDeparture(Event &evt);
    virtual void ProcessEnd(Event &evt);
    virtual void ProcessProbe(Event &evt);
    TraceSource _logger;
    std::string _name;

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
    std::vector<std::function<void(BaseStation *, Event &)>> _onArrival;
    std::vector<std::function<void(BaseStation *, Event &)>> _onDeparture;

  public:
    BaseStation(std::string name);
    virtual void Process(Event &event);

    std::string Name() const
    {
        return _name;
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
        _onDeparture.push_back(fnc);
    }

    template <typename F> void OnArrival(F &&fnc)
    {
        _onArrival.push_back(fnc);
    }
};

class Station : public BaseStation
{
  protected:
    DataCollector collector;

    int _stationIndex{};
    double _regPointClock = 0.0;

  public:
    virtual void Initialize();
    void Update();
    DataCollector &Data()
    {
        return collector;
    }
    virtual void Reset();
    Station(std::string name, int station, bool registerCollector = false);

    int stationIndex() const
    {
        return _stationIndex;
    }

    void ProcessProbe(Event &evt) override;
};
