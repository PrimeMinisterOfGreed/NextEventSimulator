#pragma once

#include "FCFSStation.hpp"


class CPU : public FCFSStation {



public:
    CPU(ILogEngine *logger, IScheduler *scheduler);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;

};