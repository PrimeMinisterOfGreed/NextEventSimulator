#include "IOStation.hpp"
#include "FCFSStation.hpp"
#include "Enums.hpp"

void IOStation::ProcessArrival(Event *evt)
{
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


}





