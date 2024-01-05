#pragma once

#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "rngs.hpp"
#include <memory>
#include <optional>

class Cpu : public Station, public IQueueHolder
{

  private:
    std::optional<Event> _eventUnderProcess;

    IScheduler *_scheduler;
    double _timeSlice;

    sptr<BaseStream> _burst;

  public:
    Cpu(IScheduler *scheduler);

    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};