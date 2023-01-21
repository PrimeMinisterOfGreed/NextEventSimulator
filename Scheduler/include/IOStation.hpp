#pragma once
#include "FCFSStation.hpp"

class IOStation : public FCFSStation
{
    
public:
    IOStation(ILogEngine *logger, IScheduler *scheduler, int stationIndex);

    void ProcessArrival(Event *evt) override;
    void ProcessDeparture(Event *evt) override;


};
