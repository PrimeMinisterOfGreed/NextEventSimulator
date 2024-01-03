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
Cpu::Cpu(IScheduler *scheduler)
    : Station("CPU", Stations::CPU), _scheduler(scheduler), _timeSlice(SystemParameters::Parameters().cpuQuantum)
{
    if (SystemParameters::Parameters().cpuUseNegExp)
    {
        auto negExpStream = new VariableStream(3, [](auto rng) { return Exponential(27); });
        _burst = sptr<BaseStream>(negExpStream);
    }
    else
    {
        auto hyperExpStream = new CompositionStream{
            4, {0.95, 0.05}, [](auto rng) { return Exponential(10); }, [](auto rng) { return Exponential(350); }};
        _burst = sptr<BaseStream>(hyperExpStream);
    }
}

void Cpu::ProcessArrival(Event &evt)
{
    // it's a new process

    if (evt.SubType != 'E')
    {
        Station::ProcessArrival(evt);
        _logger.Transfer("New process joined: {}", evt);
        evt.SubType = 'E';
        evt.ServiceTime = (*_burst)();
        if (_eventUnderProcess.has_value() && _readyQueue.Count() > 0)
        {
            _readyQueue.Push(evt);
            return;
        }
    }

    // it was in the ready queue
    else
    {
        if (_eventUnderProcess.has_value())
        {
            panic(fmt::format("Expected empty underprocess but was {}", _eventUnderProcess.value()));
        }
        _logger.Transfer("Now Processing:{}, Remaining service time:{}", evt, evt.ServiceTime);
    }
    auto slice = evt.ServiceTime > _timeSlice ? _timeSlice : evt.ServiceTime;
    evt.Type = DEPARTURE;
    evt.OccurTime = _clock + slice;
    evt.ServiceTime -= slice;
    _eventUnderProcess = evt;
    _scheduler->Schedule(evt);
}

void Cpu::ProcessDeparture(Event &evt)
{
    static CompositionStream router(
        3, SystemParameters::Parameters().cpuChoice, [](auto rng) { return Stations::IO_1; },
        [](auto rng) { return Stations::IO_2; }, [](auto rng) { return Stations::SWAP_OUT; },
        [](auto rng) { return Stations::CPU; });
    // process has finished

    if (evt.ServiceTime == 0)
    {
        Station::ProcessDeparture(evt);

        evt.Type = ARRIVAL;
        evt.Station = router();
        evt.SubType = 0;
        _scheduler->Schedule(evt);
    }
    else
    {
        _readyQueue.Enqueue(evt);
    }
    if (_readyQueue.Count() > 0)
    {
        auto newEvt = _readyQueue.Dequeue();
        newEvt.Type = ARRIVAL;
        newEvt.OccurTime = _clock;
        _scheduler->Schedule(newEvt);
    }
    _eventUnderProcess.reset();
}
