#include "RepairStation.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include <stdexcept>

void RepairStation::ProcessMaintenance(Event *evt)
{
    _logger->TraceTransfer("Processing Maintenance for station:{}", _stationIndex);
    _eventQueue.Push(evt);
    _maintenance = true;
}

void RepairStation::Reset()
{
    FCFSStation::Reset();
    _maintenance = false;
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
        if (evt->Name == "MAINTENANCE")
        {
            _maintenance = false;
            _scheduler->Schedule(new Event("MAINTENANCE", EventType::MAINTENANCE, _clock ,_clock + 480 * 60, 960 * 60, _clock, _stationIndex));
        }
        else if (_eventUnderProcess->Type != EventType::MAINTENANCE)
        {
            auto oldEvt = _eventUnderProcess;
            _eventUnderProcess = &_eventQueue.Dequeue();
            _eventUnderProcess->ArrivalTime = _clock;
            _eventUnderProcess->CreateTime = _clock;
            _eventUnderProcess->OccurTime = _clock + _eventUnderProcess->ServiceTime - (_clock - _eventUnderProcess->OccurTime);
            _eventUnderProcess->Type = EventType::DEPARTURE;
            _scheduler->Schedule(_eventUnderProcess);
        }
        else
            throw std::invalid_argument("Processing a departure for an event during maintenance");
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
