#pragma once
#include <cstddef>
#include <iostream>

enum class EventType : char
{
    NO_EVENT = -1,
    ARRIVAL = 'A',
    DEPARTURE = 'D',
    END = 'E',
    PROBE = 'P',
    MAINTENANCE = 'M'
};

struct Event
{

    static int GeneratedNodes;
    static int DeletedNodes;
    double CreateTime = 0.0;
    double OccurTime = 0.0;
    double ServiceTime = 0.0;
    double ArrivalTime = 0.0;

    std::string Name{};
    EventType Type = EventType::NO_EVENT;
    Event()
    {
    }
    Event(std::string name, EventType type, double createTime, double occurTime, double serviceTime, double arrivalTime)
        : Name{name}, Type{type}, CreateTime{CreateTime}, OccurTime{occurTime}, ServiceTime{serviceTime},
          ArrivalTime{arrivalTime}
    {

        Event::GeneratedNodes++;
    }
    ~Event()
    {
        Name.clear();
        DeletedNodes++;
    }

    Event(const Event &e)
        : CreateTime{e.CreateTime}, ArrivalTime{e.ArrivalTime},
          ServiceTime{e.ServiceTime}, Name{e.Name}, OccurTime{e.OccurTime}
    {
    }

    Event(Event &&e) : Event{e}
    {
        Event::DeletedNodes++;
        delete &e;
    }
};