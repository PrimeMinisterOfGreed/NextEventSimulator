#include "IOStation.hpp"
#include "FCFSStation.hpp"
#include "Enums.hpp"
#include "Options.hpp"


void IOStation::ProcessArrival(Event *evt)
{
    evt->ServiceTime = _serviceTime->GetValue();
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
                                                                                                stationIndex) {
    if(stationIndex == 4)
        _serviceTime = new NegExpVariable(25, streamGenerator.get()); // TODO check that those value respect the right measure (sec)
    else if(stationIndex == 5)
        _serviceTime = new NegExpVariable(5.555555556,streamGenerator.get());
}





