#include "DataProvider.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <boost/math/distributions/students_t.hpp>
#include <boost/program_options.hpp>
#include <boost/random/student_t_distribution.hpp>
#include <vector>

double idfStudent(double df, double quantile)
{
    boost::math::students_t stud{df};
    return boost::math::quantile(stud, quantile);
}

int main()
{
    ConsoleLogEngine::CreateInstance(1, "log.txt");
    int endTime = 20000;

    std::vector<StationStatistic> repairStationStatistics{};
    MachineRepairman &repairman = *new MachineRepairman(
        ConsoleLogEngine::Instance(), new DoubleStreamNegExpRandomDataProvider(endTime, 0.1, 0.14));
    for (int i = 0; i < 1; i++)
    {
        PlantSeeds(123456789 + i + 1);
        repairman.Execute();
        auto stats = repairman.GetStats();
        repairStationStatistics.push_back(stats[0]);
        repairman.Reset();
        ConsoleLogEngine::Instance()->Finalize();
    }

    ConsoleLogEngine::Instance()->TraceResult("data:{}", idfStudent(9, 0.95));
    ConsoleLogEngine::Instance()->Finalize();
}