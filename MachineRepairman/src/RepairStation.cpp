#include "RepairStation.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include <stdexcept>

RepairStation::RepairStation(IScheduler *scheduler, std::string name, int index) : FCFSStation(scheduler, name, index)
{
}

void RepairStation::ProcessDeparture(Event &evt)
{
    FCFSStation::ProcessDeparture(evt);
    if (evt.Name == "MAINTENANCE")
        return;
    evt.Station = 0;
    evt.Type = ARRIVAL;
    _scheduler->Schedule(evt);
}

void RepairStation::ProcessProbe(Event &evt)
{
    FCFSStation::ProcessProbe(evt);
    for (auto &measure : Data().GetAccumulators())
    {
        _logger.Result("{}:[{}]", measure->Name(), measure->Csv());
    }
}
