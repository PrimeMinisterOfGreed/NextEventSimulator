#pragma once
#include "FCFSStation.hpp"
#include "rngs.hpp"
#include <memory>

class IOStation : public FCFSStation
{
private:
    std::unique_ptr<VariableStream>  _serviceTime;
public:
    IOStation(ILogEngine *logger, IScheduler *scheduler, int stationIndex);
    void ProcessArrival(Event&evt) override;
    void ProcessDeparture(Event&evt) override;


};
