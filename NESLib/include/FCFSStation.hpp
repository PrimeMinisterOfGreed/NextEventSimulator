/**
 * @file FCFSStation.hpp
 * @author matteo.ielacqua
 * @brief header per la stazione FCFS, maggiori informazioni in @see FCFSStation.cpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Collections/LinkedList.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Station.hpp"
#include <optional>
#include <queue>
#include <vector>

class FCFSStation : public Station, public IQueueHolder
{
  protected:
    std::optional<Event> _eventUnderProcess;
    IScheduler *_scheduler;

  public:
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
    void ProcessEnd(Event &evt) override;
    void ProcessProbe(Event &evt) override;
    void Reset() override;
    FCFSStation(IScheduler *scheduler, std::string name, int stationIndex);
};
