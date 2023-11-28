#pragma once
#include <cstddef>
#include <fmt/core.h>
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
    Event(std::string name, char type, double createTime, double occurTime, double serviceTime, double arrivalTime,
          int stationTarget = 0)
        : Name{name}, Type{type}, CreateTime{createTime}, OccurTime{occurTime}, ServiceTime{serviceTime},
          ArrivalTime{arrivalTime}, Station(stationTarget)
    {
        Event::GeneratedNodes++;
    }
    ~Event()
    {
        Name.clear();
        DeletedNodes++;
    }

    Event(const Event &) = default;

    bool operator==(Event &oth);
};

template <> struct fmt::formatter<Event>
{
    std::string fmt = "";
    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator
    {
        for (auto &c : ctx)
        {
            fmt += c;
        }
        fmt.erase(fmt.size() - 1);
        return ctx.end();
    }

    auto format(const Event &list, format_context &ctx) -> format_context::iterator
    {
        return fmt::format_to(ctx.out(), "{}", fmt);
    }
};
