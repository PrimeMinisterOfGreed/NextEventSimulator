#include <boost/program_options.hpp>
#include "DataProvider.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "rngs.hpp"

int main()
{
    ConsoleLogEngine::CreateInstance(3, "log.txt");
    int endTime = 20000;
    PlantSeeds(123456789);
    MachineRepairman &repairman =
        *new MachineRepairman(ConsoleLogEngine::Instance(), new DoubleStreamNegExpRandomDataProvider(endTime, 0.1, 0.14), endTime);
    repairman.Execute();
    ConsoleLogEngine::Instance()->Finalize();
}