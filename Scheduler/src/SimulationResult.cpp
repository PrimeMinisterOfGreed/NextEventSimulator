#include "SimulationResult.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "SystemParameters.hpp"
#include <fmt/core.h>

void ConfidenceHits::Accumulate(bool x_in, bool u_in, bool n_in, bool w_in, bool activeTime_in)
{
    throughput_in = x_in;
    utilization_in = u_in;
    meanClients_in = n_in;
    meanWaits_in = w_in;
    activeTime_in = activeTime_in;
    throughput += x_in;
    utilization += u_in;
    meanclients += n_in;
    meanwaits += w_in;
    activeTimes += activeTime_in;
}

void SimulationResult::AccumulateResult(std::map<std::string, Accumulator<>[4]> accumulators,Accumulator<> activeTime ,int seed)
{
    if (!mva.inited)
        mva.PreloadModel();
    for (auto value : accumulators)
    {
        auto station = value.first;
        bool found = false;
        for(auto s : mva.Stations()){
            if(s == station) {
                found = true;
                break;
            }
        }
        if(!found) return;
        int n = SystemParameters::Parameters().numclients;
        double t_throughput = mva.Throughputs(station)[n];
        double t_utilization = mva.Utilizations(station)[n];
        double t_meanclients = mva.MeanClients(station)[n];
        double t_meanwait = mva.MeanWaits(station)[n];
        double t_activeTime = mva.ActiveTimes()[n];

        auto ref = accumulators[station];

        _confidenceHits[station].Accumulate(
            ref[0].confidence().isInTval(t_throughput), ref[1].confidence().isInTval(t_utilization),
            ref[2].confidence().isInTval(t_meanclients), ref[3].confidence().isInTval(t_meanwait),activeTime.confidence().isInTval(t_activeTime));
        seeds.push_back(seed);
    }
}



SimulationResult::SimulationResult()
{
}

void SimulationResult::AddShellCommands(SimulationShell *shell)
{
    shell->AddCommand("lsimresults", [this](SimulationShell *shell, const char *context) {
        for (auto s : mva.Stations())
        {
            shell->Log()->Result("Station:{}\n{}", s, _confidenceHits[s]);
        }
    });
}

auto format_as(ConfidenceHits b)
{
    return fmt::format(
        "Name   Hits   Last\n Throughput {}  {} \n Utilization  {}  {} \n MeanWaits  {}  {}\n MeanClients  {}   {}\n ActiveTime {} {}",
        b.throughput, b.throughput_in, b.utilization, b.utilization_in, b.meanwaits, b.meanWaits_in, b.meanclients,
        b.meanClients_in,b.activeTimes, b.activeTime_in);
}
