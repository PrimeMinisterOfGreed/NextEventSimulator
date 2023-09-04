#pragma once

#include "FCFSStation.hpp"


class Cpu : public Station {

private:
    Event * _eventUnderProcess;
    IScheduler * _scheduler;
    double _timeSlice;
    EventList& _eventQueue = *new EventList();
    void ManageProcess(Event * evt, double burst);
    double Burst(double alpha, double beta ,double u1, double u2);

public:
    Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;
    void Reset() override;

};