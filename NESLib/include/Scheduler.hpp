#pragma once
#include "Collections/LinkedList.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "Station.hpp"
#include "Usings.hpp"
#include <memory>
#include <optional>
#include <vector>

class Scheduler : public IScheduler, public BaseStation
{
  protected:
    std::vector<sptr<Station>> _stations{};
    DoubleLinkedList<Event> _eventList{};
    virtual bool Route(Event event);

  public:
    virtual void Schedule(Event event) override;
    virtual void Initialize();

    template <typename STAT> inline Scheduler &AddStation(STAT *station)
    {
        _stations.push_back(sptr<STAT>(station));
        return *this;
    };

    Scheduler(std::string name) : BaseStation(name)
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

    std::optional<sptr<Station>> operator[](int index)
    {
        return _stations[index];
    }
    Event ProcessNext();
    void Reset() override;
    virtual Event Create(double interArrival, double serviceTime, int stationTarget = -1, EventType type = ARRIVAL);
};
