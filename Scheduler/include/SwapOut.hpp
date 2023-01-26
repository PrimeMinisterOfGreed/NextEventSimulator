//
// Created by drfaust on 23/01/23.
//
#pragma once

#include "Station.hpp"
#include "ISimulator.hpp"

class SwapOut : public Station
{
protected:
    IScheduler * _scheduler;
    RandomVariable * _swap;
public:
    SwapOut(ILogEngine *logger, IScheduler * scheduler );

protected:
    void ProcessArrival(Event *evt) override;
    void ProcessDeparture(Event *evt) override;

};