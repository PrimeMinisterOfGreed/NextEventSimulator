#pragma once

#include "FCFSStation.hpp"
#include "rngs.hpp"
#include <memory>


class Cpu : public Station {

private:
    Event * _eventUnderProcess;
    std::unique_ptr<VariableStream> burst;
    std::unique_ptr<VariableStream> serviceTime;
    double alpha = 0.95;
    double beta = 0.05;
    double u1= 0.01;
    double u2= 0.35;
    IScheduler * _scheduler;
    double _timeSlice;
    EventList _eventQueue{};
    void ManageProcess(Event * evt, double burst);
    double Burst(double alpha, double beta ,double u1, double u2);

public:
    Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice);

    void ProcessArrival(Event *evt) override;

    void ProcessDeparture(Event *evt) override;
    void Reset() override;

};