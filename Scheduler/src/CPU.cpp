#include "CPU.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "Usings.hpp"
#include "rngs.hpp"
#include "rvgs.h"
#include <fmt/core.h>
#include <vector>

// burst is written this way because of this
// https://rossetti.github.io/RossettiArenaBook/app-rnrv-rvs.html#AppRNRV:subsec:MTSRV
Cpu::Cpu(IScheduler *scheduler) : Station("CPU", Stations::CPU), _scheduler(scheduler)
{
}

void Cpu::ProcessArrival(Event &evt)
{

    Station::ProcessArrival(evt);
    evt.ServiceTime = Burst();
    _logger.Debug("Assigned burst time {}", evt.ServiceTime);
    if (_eventUnderProcess.has_value())
    {
        _eventList.Enqueue(evt);
        _logger.Debug("CPU is busy, enqueue process {}, queue size {}", evt, _eventList.Count());
        return;
    }
    Manage(evt);
}

void Cpu::Manage(Event &evt)
{
    static VariableStream sliceStream{99,
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().cpuQuantum); }};

    auto quantum = SystemParameters::Parameters().slicemode == SystemParameters::FIXED
                       ? SystemParameters::Parameters().cpuQuantum
                       : sliceStream();
    auto slice = quantum < evt.ServiceTime ? quantum : evt.ServiceTime;
    evt.Type = DEPARTURE;
    evt.ServiceTime -= slice;
    evt.OccurTime = _clock + slice;
    _scheduler->Schedule(evt);
    _eventUnderProcess = evt;
}

void Cpu::ProcessDeparture(Event &evt)
{

    static Router router(4, SystemParameters::Parameters().cpuChoice, {IO_1, IO_2, SWAP_OUT, CPU});
    // process has finished
    core_assert(_eventUnderProcess.has_value(), "Event {} in departure but no event under process", evt);
    core_assert(evt == _eventUnderProcess.value(), "Event {} scheduled for departure but other event in process {}",
                evt, _eventUnderProcess.value());
    _eventUnderProcess.reset();
    if (evt.ServiceTime == 0)
    {
        evt.Type = ARRIVAL;
        evt.Station = router();
        evt.SubType = 0;
        _scheduler->Schedule(evt);
        Station::ProcessDeparture(evt);
    }
    else
    {
        _eventList.Enqueue(evt);
        _completions++;
    }
    if (_eventList.Count() > 0)
    {
        auto newEvt = _eventList.Dequeue();
        newEvt.OccurTime = _clock;
        Manage(newEvt);
        _logger.Debug("Managing {} from ready queue, remainig SV time {}", newEvt, newEvt.ServiceTime);
    }
}

double Cpu::Burst()
{
    static CompositionStream hyperExp{3,
                                      {SystemParameters::Parameters().alpha, SystemParameters::Parameters().beta},
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().u1); },
                                      [](auto rng) { return Exponential(SystemParameters::Parameters().u2); }};

    return hyperExp();
}
