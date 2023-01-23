#pragma once

#include "FCFSStation.hpp"
#include "Generators.hpp"

class Cpu : public FCFSStation {

private:
    NegExpVariable * _firstStage;
    NegExpVariable * _secondStage;
    RandomVariable * _routing;
public:
    Cpu(ILogEngine *logger, IScheduler *scheduler);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;

};