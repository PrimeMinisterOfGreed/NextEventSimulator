#pragma once
#include "Collections/LinkedList.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Station.hpp"
#include "Usings.hpp"
#include <vector>

class Scheduler : public IScheduler, public Station
{
  protected:
    std::vector<sptr<Station>> _stations{};
    DoubleLinkedList<Event> _eventList{};
    virtual bool Route(Event event);

  public:
    virtual void Schedule(Event event) override;
    virtual void Initialize() override;
    Scheduler(std::string name) : Station(name, 0)
    {
    }
    std::optional<sptr<Station>> operator[](std::string name)
    {
        for (auto &st : _stations)
        {
            if (st->Name() == name)
            {
                return st;
            }
        }
        return {};
    }

    virtual Event Create(double interArrival, double serviceTime, int stationTarget = 0, EventType type = ARRIVAL);
};
