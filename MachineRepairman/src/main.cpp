#include "DataProvider.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "MachineRepairman.hpp"
#include "Station.hpp"
#include "rngs.hpp"
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <boost/program_options.hpp>
#include <boost/random/student_t_distribution.hpp>
#include <cmath>
#include <vector>
#include "Statistics.hpp"

double idfStudent(double df, double quantile)
{
    boost::math::students_t stud{df};
    return boost::math::quantile(stud, quantile);
}

int main()
{
    ConsoleLogEngine::CreateInstance(1, "log.txt");
    int endTime = 20000;
    int sample = 100;
    StatisticCollector & collector = *new StatisticCollector();
    MachineRepairman &repairman = *new MachineRepairman(
        ConsoleLogEngine::Instance(), new DoubleStreamNegExpRandomDataProvider(endTime, 0.000185185, 0.001111111));
    for (int i = 0; i < sample; i++)
    {
        PlantSeeds(123456789 + i + 1);
        repairman.Execute();
        auto stats = repairman.GetStats();
        collector.Accumulate(stats[0]);
        repairman.Reset();
        ConsoleLogEngine::Instance()->Finalize();
    }



    double sampleMean =  boost::accumulators::mean(collector.avgWaiting());
    ConsoleLogEngine::Instance()->TraceResult("Mean of waits:{}",sampleMean);

    double interval =
        idfStudent(sample - 1, 0.95) * (boost::accumulators::variance(collector.avgWaiting()) / sqrt(sample - 1));

    ConsoleLogEngine::Instance()->TraceResult("Delta interval:{} ", interval);
    ConsoleLogEngine::Instance()->TraceResult("Mean waiting interval: {}<=u<={}", sampleMean - interval, sampleMean+interval);
    ConsoleLogEngine::Instance()->Finalize();

}