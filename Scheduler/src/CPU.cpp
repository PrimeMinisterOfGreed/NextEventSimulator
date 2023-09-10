#include "CPU.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "OperativeSystem.hpp"
#include "Options.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include "rvgs.h"

Cpu::Cpu(IScheduler *scheduler) : Station("CPU", Stations::CPU), _scheduler(scheduler)
{

    burst = RandomStream::Global().GetStream([this](RandomStream &gen) -> double {
        auto x = RandomStream::Global().Random();
        return alpha * (1 / u1) * exp(-x / u1) + beta * (1 / u2) * exp(-x / u2);
    });
    _timeSlice = SystemParameters::Parameters().cpuQuantum;
    serviceTime = RandomStream::Global().GetStream(
        [this](auto &gen) { return Exponential(SystemParameters::Parameters().averageCpuTime); });
    _name = "CPU";
}

void Cpu::ProcessArrival(Event &evt)
{
    if (evt.SubType == EventType::NO_EVENT)
    {
        Station::ProcessArrival(evt);
        evt.SubType = 'C';
        double processServiceTime = (*serviceTime)();
        evt.ServiceTime = processServiceTime;
    }
    if (!_eventUnderProcess.has_value())
    {
        ManageProcess(evt, (*burst)());
        _scheduler->Schedule(evt);
    }
    else
    {
        _eventQueue.Enqueue(evt);
    }
}

void Cpu::ProcessDeparture(Event &evt)
{
    if (evt.ServiceTime > 0)
    {
        evt.Type = EventType::ARRIVAL;
        evt.OccurTime = _clock;
        if (_sysClients > 1)
        {
            auto newEvt = _eventQueue.Dequeue();
            ManageProcess(newEvt, (*burst)());
            _eventUnderProcess.emplace(newEvt);
            _eventQueue.Enqueue(evt);
            _scheduler->Schedule(newEvt);
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
        double num = Uniform(0.0, 1.0);
        int selected = 0;
        for (int i = 0; i < 3; i++)
            if (num >= probabilities[i])
                selected = i;
        evt.ArrivalTime = _clock;
        evt.OccurTime = _clock;
        evt.Type = EventType::ARRIVAL;
        switch (selected)
        {
        case 0:
            evt.Station = Stations::IO_1;
            break;

        case 1:
            evt.Station = Stations::IO_2;
            break;

        case 2:
            evt.Station = Stations::SWAP_OUT;
            break;
        }
        _scheduler->Schedule(evt);
    }
}

void Cpu::ManageProcess(Event &evt, double burst)
{
    evt.ArrivalTime = _clock;
    evt.CreateTime = _clock;
    if (burst < evt.ServiceTime)
    {
        evt.OccurTime = _clock + burst;
        evt.ServiceTime -= burst;
    }
    else
    {
        evt.OccurTime = _clock + evt.ServiceTime;
        evt.ServiceTime = 0;
    }
    evt.Type = EventType::DEPARTURE;
}

void Cpu::Reset()
{
    Station::Reset();
    _eventQueue.Clear();
}
