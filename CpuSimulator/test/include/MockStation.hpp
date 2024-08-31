#pragma once

#include "Event.hpp"
#include "Station.hpp"
struct MockStation : public Station
{
    MockStation(int station) : Station("mockStation", station)
    {
    }

    virtual void Process(Event &evt) override
    {
    }
};