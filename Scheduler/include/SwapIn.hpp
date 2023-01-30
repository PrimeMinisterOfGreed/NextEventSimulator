#include "FCFSStation.hpp"

#pragma once

class SwapIn : public FCFSStation
{
public:
    SwapIn(ILogEngine *logger, IScheduler *scheduler);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;

private:
    NegExpVariable& _serviceTime;
public:

};
