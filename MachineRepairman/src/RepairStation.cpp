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

void RepairStation::ProcessProbe(Event &evt)
{
    Station::ProcessProbe(evt);
}

void RepairStation::ProcessMaintenance(Event &evt)
{
    
}
