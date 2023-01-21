#pragma once
#include "FCFSStation.hpp"



class ReserveStation : FCFSStation
{
  protected:

  public:
    void ProcessArrival(Event *evt) override;
    void ProcessDeparture(Event *evt) override;
};
