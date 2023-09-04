#include "CPU.hpp"
#include "Enums.hpp"
#include "rngs.h"
#include "OperativeSystem.hpp"
#include "Options.hpp"


Cpu::Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice) : Station(logger, Stations::CPU),
                                                                        _timeSlice(timeSlice),
                                                                        _scheduler(scheduler),
    _burst{ *new DoubleStageHyperExpVariable(0.95, 0.05, 0.01, 0.35, streamGenerator) },
    _processServiceTime{ *new NegExpVariable(1 / (timeSlice / 1000), streamGenerator)},
    _routing{ *new RandomVariable(streamGenerator) }
{
    _name = "CPU";
}


void Cpu::ProcessArrival(Event *evt)
{
    double burst = _burst();
    if (evt->SubType == EventType::NO_EVENT)
    {
        Station::ProcessArrival(evt);
        evt->SubType = 'C';
        double processServiceTime = _processServiceTime();
        evt->ServiceTime = processServiceTime;
    }
    if (_eventUnderProcess == nullptr)
    {
        ManageProcess(evt, burst);
        _scheduler->Schedule(evt);
    }
    else
    {
        _eventQueue.Enqueue(evt);
    }
}

void Cpu::ProcessDeparture(Event *evt)
{
    if (evt->ServiceTime > 0)
    {
        evt->Type = EventType::ARRIVAL;
        evt->OccurTime = _clock;
        if (_sysClients > 1)
        {
            _eventQueue.Enqueue(evt);
        }
        else
        {

            _scheduler->Schedule(evt);
        }
    }
    else
    {
        Station::ProcessDeparture(evt);
        double probabilities[3] = {0, 0.65, 0.9};
        double num = _routing();
        int selected = 0;
        for (int i = 0; i < 3; i++)
            if (num >= probabilities[i])
                selected = i;
        evt->ArrivalTime = _clock;
        evt->OccurTime = _clock;
        evt->Type = EventType::ARRIVAL;
        switch (selected)
        {
            case 0:
                evt->Station = Stations::IO_1;
                break;

            case 1:
                evt->Station = Stations::IO_2;
                break;

            case 2:
                evt->Station = Stations::SWAP_OUT;
                break;
        }
        _scheduler->Schedule(evt);
    }
    if (_sysClients > 0)
    {
        auto nextEvt = &_eventQueue.Dequeue();
        ManageProcess(nextEvt,_burst());
        _scheduler->Schedule(nextEvt);
    }
}

void Cpu::ManageProcess(Event *evt, double burst)
{
    evt->ArrivalTime = _clock;
    evt->CreateTime = _clock;
    if (burst < evt->ServiceTime)
        evt->OccurTime = _clock + burst;
    else
        evt->OccurTime = _clock + evt->ServiceTime;
    evt->ServiceTime -= burst;
    evt->Type = EventType::DEPARTURE;
}

void Cpu::Reset()
{
    Station::Reset();
    _eventQueue.Clear();
}
