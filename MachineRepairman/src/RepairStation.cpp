#include "RepairStation.hpp"
#include "FCFSStation.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include <stdexcept>

RepairStation::RepairStation(IScheduler *scheduler, std::string name, int index) : FCFSStation(scheduler, name, index)
{
}
