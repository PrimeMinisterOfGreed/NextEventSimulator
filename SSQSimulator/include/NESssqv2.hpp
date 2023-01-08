#pragma once
#include "DataProvider.hpp"
#include "LogEngine.hpp"
#include "Event.hpp"
#include "LinkedList.hpp"
#include "ssq1.hpp"
#include <iostream>

extern int endTime;
extern int targetTime;

class EventList : public DoubleLinkedList<Event>
{
  public:
    virtual const std::string &ToString();
};

struct Statistics
{
    int nsys;     /* Number of customers in system */
    int nsysMax;  /* Maximum numbwr of customers in system */
    int narr;     /* Number of Arrivals */
    int ncom;     /* Number of Completions */
    int ndelayed; /* Number of Delayed customers */
    int n_target;
    int event_counter; /* Number of the events processed by the simulkator*/
    int node_number;   /* actual number of nodes used by the simulator */
    int return_number; /* Number of nodes used by the simulator */
    int clientTarget;  /* number of client probed at target time*/

    double Area_n; /* Integral of the function nsys(t)  = Area_w*/
    double Area_d; /* Integral of the function nqueue(t)  = Area_w*/

    double Last_arrival; /* Timeof last arrival */

    double E_of_sigma;  /* Average inter-arrival time */
    double E_of_S;      /* Average service time */
    double E_of_D;      /* Average delay time */
    double E_of_W;      /* Average wait time */
    double U;           /* Utilization */
    double X;           /* Throughput */
    double Theta;       /* Input rate */
    double Lambda;      /* Arrival rate */
    double Mu;          /* Arrival rate */
    double Rho;         /* Traffic intensity */
    double E_of_q;      /* Average number of customers in queue */
    double E_of_y;      /* Average number of customers in service */
    double E_of_n;      /* Average number of customers at server */
    double Target_time; /* maximum simulation time */
    std::string &ToString();
};

struct Clock
{
    double Start;        /* Beginnig of Observation Period */
    double Stop;         /* End of Observation Period */
    double ObservPeriod; /* Length of the Observation Period */
    double clock;        /* Clock of the simulator - Simulation time */
    double oldclock;
    double lasttime; /* time of last processed event*/
    double End_time; /* maximum simulation time */
    double Busy;     /* Busy time */
    double Arrival;  // time of arrival from provider
    double Service;  // time of service from provider

    std::string &ToString();
};

class NESssq : public ISSQSimulator
{
  private:
    Clock &_clock = *new Clock();
    Statistics &_statistic = *new Statistics();

    EventList &_futureEvents = *new EventList{};
    EventList &_inputQueue = *new EventList{};
    EventList &_available = *new EventList{};
    IDataProvider *_provider;
    bool _stop = false;
    bool _printEventStack = false;

  protected:
    virtual void Initialize();
    void Arrival(Event &e);
    void Schedule(Event &e);
    void Departure(Event &e);
    void End(Event &e);
    void Probe(Event &e);
    ILogEngine* _logger;
    Accumulator systemClients{};

  public:
    virtual void Execute() override;
    virtual void Engine();
    virtual void Report() override;
    NESssq(IDataProvider *provider, ILogEngine *logger)
        : _provider{provider}, _logger(logger)
    {
    }
};