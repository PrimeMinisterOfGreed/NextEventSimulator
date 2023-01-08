#pragma once
#include "Event.hpp"


class Station
{
  private:
    std::string _name;

    int _arrivals;
    int _completions;
    double _busyTime;
    double _areaN;
    double _areaS;
  public:
    void ProcessArrival(Event &evt);
    void ProcessDeparture(Event &evt);
};