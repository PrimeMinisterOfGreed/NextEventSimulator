#pragma once
#include <cstddef>
#include <iostream>

enum EventType : char
{
    NO_EVENT = 'N',
    ARRIVAL = 'A',
    DEPARTURE = 'D',
    END = 'E',
    PROBE = 'P',
    MAINTENANCE = 'M'
};

struct Event
{
    int Station = 0;
    static int GeneratedNodes;
    static int DeletedNodes;
    double CreateTime = 0.0;
    double OccurTime = 0.0;
    double ServiceTime = 0.0;
    double ArrivalTime = 0.0;
    std::string Name{};
    char Type = EventType::NO_EVENT;
    char SubType = EventType::NO_EVENT;
    Event()
    {
    }
    Event(std::string name, char type, double createTime, double occurTime, double serviceTime, double arrivalTime, int stationTarget = 0)
        : Name{name}, Type{type}, CreateTime{createTime}, OccurTime{occurTime}, ServiceTime{serviceTime},
          ArrivalTime{arrivalTime},Station(stationTarget)
    {
        Event::GeneratedNodes++;
    }
    ~Event()
    {
        Name.clear();
        DeletedNodes++;
    }

    Event(const Event&) = default;

    bool operator==(Event& oth);
};