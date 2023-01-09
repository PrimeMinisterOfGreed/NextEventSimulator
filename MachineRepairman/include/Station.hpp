#pragma once
#include "Event.hpp"
#include "LogEngine.hpp"

enum class StationEventType : char
{
    MAINTENANCE = 'M'
};

struct StationEvent : public Event
{
};

struct StationStatistic
{
    double E_of_sigma;
    double E_of_S;
    double E_of_D;
    double E_of_W;
    double U;
    double X;
    double Theta;
    double Lambda;
    double Mu;
    double Rho;
    double E_of_q;
    double E_of_y;
    double E_of_n;
    std::string &ToString() const;
};

class Station
{
  protected:
    std::string _name;
    int _arrivals;
    int _completions;
    int _sysClients;
    int _maxClients;
    double _busyTime;
    double _observationPeriod;
    double _lastArrival;
    double _areaN;
    double _areaS;
    double _oldclock = 0.0;
    double _clock = 0.0;
    virtual void ProcessArrival(Event &evt);
    virtual void ProcessDeparture(Event &evt);
    virtual void ProcessEnd(Event &evt);
    virtual void ProcessProbe(Event &evt);
    ILogEngine *_logger;

  public:
    void Process(Event &event);
    virtual std::string &ToString();
    StationStatistic GetStatistics();
    Station(ILogEngine *logger);
};