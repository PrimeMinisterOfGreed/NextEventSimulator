#include "SimulationResult.hpp"
#include "Shell/SimulationShell.hpp"
#include "SystemParameters.hpp"

void ConfidenceHits::Accumulate(bool x_in, bool u_in, bool n_in, bool w_in)
{
    throughput_in = x_in;
    utilization_in = u_in;
    meanClients_in = n_in;
    meanWaits_in = w_in;
    throughput += x_in;
    utilization += u_in;
    meanclients += n_in;
    meanwaits += w_in;
}

void SimulationResult::AccumulateResult(std::map<std::string, Accumulator<>[4]> accumulators, int seed)
{
    for (auto value : accumulators)
    {
        auto station = value.first;
        if (std::find(mva.Stations().begin(), mva.Stations().end(), station) == mva.Stations().end())
            continue;
        int n = SystemParameters::Parameters().numclients;
        double t_throughput = mva.Throughputs(station)[n];
        double t_utilization = mva.Utilizations(station)[n];
        double t_meanclients = mva.MeanClients(station)[n];
        double t_meanwait = mva.MeanWaits(station)[n];
        auto ref = accumulators[station];

        _confidenceHits[station].Accumulate(
            ref[0].confidence().isInTval(t_throughput), ref[1].confidence().isInTval(t_utilization),
            ref[2].confidence().isInTval(t_meanclients), ref[3].confidence().isInTval(t_meanwait));
    }
}

void SimulationResult::AddShellCommands(SimulationShell *shell)
{
    shell->AddCommand("lsimresults", [](SimulationShell *shell, const char *context) {
        // TODO add results display
    });
}