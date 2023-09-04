
#pragma once
#include "FCFSStation.hpp"


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
