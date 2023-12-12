#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <cmath>
#include <vector>

int main()
{
    LogEngine::CreateInstance("machinerepairman.txt");
    RandomStream::Global().PlantSeeds(123456789);
    MachineRepairman simulator{};
    simulator.Initialize();
    simulator.Execute();
}