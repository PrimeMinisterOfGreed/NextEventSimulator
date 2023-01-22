#pragma once
#include "FCFSStation.hpp"



class ReserveStation : FCFSStation
{
  protected:
    int _multiProgrammingDegree;
    NegExpVariable * _generator;
  public:
    void ProcessArrival(Event *evt) override;
    void ProcessDeparture(Event *evt) override;

};
