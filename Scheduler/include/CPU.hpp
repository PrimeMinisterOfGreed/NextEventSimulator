#pragma once

#include "Enums.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "rngs.hpp"
#include <memory>
#include <optional>

class Cpu : public Station
{

  private:
    std::optional<Event> _eventUnderProcess;

    IScheduler *_scheduler;
    double _timeSlice;
    EventList _readyQueue{};
    sptr<BaseStream> _burst;

  public:
    Cpu(IScheduler *scheduler);

    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};