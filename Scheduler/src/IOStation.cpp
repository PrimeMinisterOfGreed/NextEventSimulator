#include "IOStation.hpp"
#include "FCFSStation.hpp"
#include "Enums.hpp"
#include "Options.hpp"


void IOStation::ProcessArrival(Event *evt)
{
    evt->ServiceTime = _serviceTime();
    FCFSStation::ProcessArrival(evt);
}

void IOStation::ProcessDeparture(Event *evt)
{
    FCFSStation::ProcessDeparture(evt);
    evt->Station = Stations::CPU;
    evt->OccurTime = _clock;
    evt->Type = EventType::ARRIVAL;
    _scheduler->Schedule(evt);
}

IOStation::IOStation(ILogEngine *logger, IScheduler *scheduler, int stationIndex) : FCFSStation(logger, scheduler,
                                                                                                stationIndex),
    _serviceTime{stationIndex == 4? *new NegExpVariable(25, streamGenerator) :  *new NegExpVariable(5.555555556, streamGenerator)}
{
    _name = _stationIndex == 4 ? "IO1" : "IO2";
}





