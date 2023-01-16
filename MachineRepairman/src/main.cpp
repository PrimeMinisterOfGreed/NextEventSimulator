#include "DataProvider.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <boost/program_options.hpp>
#include <vector>

int main()
{
    ConsoleLogEngine::CreateInstance(1, "log.txt");
    int endTime = 20000;
    MachineRepairman &repairman = *new MachineRepairman(
        ConsoleLogEngine::Instance(), new DoubleStreamNegExpRandomDataProvider(endTime, 0.1, 0.14), endTime);
    std::vector<StationStatistic> repairStationStatistics{};
    for (int i = 0; i < 100; i++)
    {
        PlantSeeds(123456789 + i);
        repairman.Execute();
        auto stats = repairman.GetStats();
        repairStationStatistics.push_back(stats[0]);
        ConsoleLogEngine::Instance()->Finalize();
    }
    
}