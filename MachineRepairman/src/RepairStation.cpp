#include "RepairStation.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"

void RepairStation::ProcessMaintenance(Event *evt)
{
    _logger->TraceTransfer("Processing Maintenance for station:{}", _stationIndex);
    _eventQueue.Push(evt);
    _maintenance = true;
}

void RepairStation::Reset()
{
    FCFSStation::Reset();
    Initialize();
}

void RepairStation::Initialize()
{
    _scheduler->Schedule(new Event("MAINTENANCE", EventType::MAINTENANCE, 0, 480 * 60, 960 * 60, 0, _stationIndex));
}



void RepairStation::ProcessDeparture(Event *evt)
{
    
    if (_maintenance)
    {
        
        _scheduler->Schedule(new Event(makeformat("MAINTENANCE_S{}", _stationIndex), EventType::MAINTENANCE, _clock,
                                       _clock + 480 * 60, 960 * 60, _clock, _stationIndex));
    }
    else
    {
        FCFSStation::ProcessDeparture(evt);
    }
}

RepairStation::RepairStation(ILogEngine *logger, IScheduler *scheduler, int stationIndex)
    : FCFSStation(logger, scheduler, stationIndex)
{
}
