#pragma once

#include "FCFSStation.hpp"
#include "Generators.hpp"

class Cpu : public Station {

private:
    DoubleStageHyperExpVariable * _burst;
    NegExpVariable * _processServiceTime;
    RandomVariable * _routing;
    Event * _eventUnderProcess;
    IScheduler * _scheduler;
    double _timeSlice;
    EventList& _eventQueue = *new EventList();
    void ManageProcess(Event * evt, double burst);
public:
    Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;

    void Reset() override;

};