#include "CPU.hpp"
#include "Enums.hpp"
#include "rngs.hpp"
#include "OperativeSystem.hpp"
#include "Options.hpp"


Cpu::Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice) : Station(logger, Stations::CPU),
                                                                        _timeSlice(timeSlice),
                                                                        _scheduler(scheduler)
{
    _routing = new RandomVariable(streamGenerator);
    double filteredLambda = timeSlice / 1000;
    _processServiceTime = new NegExpVariable(1 / filteredLambda, streamGenerator);
    _burst = new DoubleStageHyperExpVariable(0.95, 0.05, 0.01, 0.35, streamGenerator);
}


void Cpu::ProcessArrival(Event *evt)
{
    Station::ProcessArrival(evt);
    double burst = _burst->GetValue();
    if (evt->SubType == EventType::NO_EVENT)
    {
        evt->SubType = 'C';
        double processServiceTime = _processServiceTime->GetValue();
        evt->ServiceTime = processServiceTime;
    }
    if (_eventUnderProcess == nullptr)
    {
        ManageProcess(evt, burst);
    }
    else
    {
        _eventQueue.Enqueue(evt);
    }
}

void Cpu::ProcessDeparture(Event *evt)
{
    Station::ProcessDeparture(evt);
    if (_sysClients > 0)
    {
        auto newEvt = &_eventQueue.Dequeue();
        ManageProcess(newEvt, _burst->GetValue());
    }
    if (evt->ServiceTime > 0)
    {
        if (_sysClients > 0)
        {
            evt->Type = EventType::ARRIVAL;
            _eventQueue.Enqueue(evt);
        }
        else
        {
            evt->Type = EventType::ARRIVAL;
            _scheduler->Schedule(evt);
        }
    }
    else
    {
        double probabilities[3] = {0.65, 0.9, 1};
        double num = _routing->GetValue();
        int selected = 0;
        for (int i = 0; i < 3; i++)
            if (num > probabilities[i])
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
    _scheduler->Schedule(evt);
}

void Cpu::Reset()
{
    Station::Reset();
    _eventQueue.Clear();
}
