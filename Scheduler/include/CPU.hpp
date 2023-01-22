#pragma once

#include "FCFSStation.hpp"


class Cpu : public FCFSStation {



public:
    Cpu(ILogEngine *logger, IScheduler *scheduler);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;

};