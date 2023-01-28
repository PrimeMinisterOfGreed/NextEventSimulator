#pragma once
#include "FCFSStation.hpp"



class ReserveStation : public FCFSStation
{
  protected:
    int _multiProgrammingDegree;
    NegExpVariable * _serviceTime;
  public:
    void ProcessDeparture(Event *evt) override;

    void ProcessArrival(Event *evt) override;

    ReserveStation(int multiProgrammingDegree, ILogEngine * logger, IScheduler * scheduler);
};
