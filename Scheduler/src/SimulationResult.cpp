#include "SimulationResult.hpp"
#include "Core.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "SystemParameters.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <vector>

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

void SimulationResult::CollectResult(int seed)
{
    if (!mva.inited)
        mva.PreloadModel();
    for (auto value : _acc)
    {
        auto station = value.first;
        bool found = false;
        for (auto s : mva.Stations())
        {
            if (s == station)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return;
        int n = SystemParameters::Parameters().numclients;
        double t_throughput = mva.Throughputs(station)[n];
        double t_utilization = mva.Utilizations(station)[n];
        double t_meanclients = mva.MeanClients(station)[n];
        double t_meanwait = mva.MeanWaits(station)[n];
        double t_activeTime = mva.ActiveTimes()[n];

        auto ref = _acc[station];

        _confidenceHits[station].Accumulate(ref[StationStats::throughput][0].confidence().isInTval(t_throughput),
                                            ref[StationStats::utilization][0].confidence().isInTval(t_utilization),
                                            ref[StationStats::meancustomer][0].confidence().isInTval(t_meanclients),
                                            ref[StationStats::meanwait][0].confidence().isInTval(t_meanwait),
                                            tgt._mean.confidence().isInTval(t_activeTime));
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
    tgt.AddShellCommands(shell);
    shell->AddCommand("lmeasures", [this](SimulationShell *shell, auto c) {
        for (auto s : _acc)
        {
            std::string result = "";
            for (int i = 0; i < StationStats::MeasureType::size; i++)
            {
                auto expected = mva.ExpectedForAccumulator(s.first, s.second[(StationStats::MeasureType)i][0]);
                result += fmt::format("{}, Expected Value:{}, Diff from conf interval:{}\n",
                                      s.second[(StationStats::MeasureType)i], expected,
                                      s.second[(StationStats::MeasureType)i].confidence().tvalDiff(expected));
            }
            shell->Log()->Result("Station:{}\n{}", s.first, result);
        }
        for (auto m : _customMeasure)
        {
            shell->Log()->Result("{}", m.second);
        }
    });
    shell->AddCommand("reset_measures", [this](auto s, auto ctx) { Reset(); });
}

void SimulationResult::Reset()
{
    for (auto &v : _acc)
    {
        v.second.Reset();
    }
    for (auto &m : _customMeasure)
    {
        m.second.WithConfidence(SimulationResult::confidence);
        m.second.MoveEsemble(1);
        m.second[0].Reset();
    }
}

void SimulationResult::Collect(BaseStation *station)
{
    _acc[station->Name()].Collect(station);
}

void SimulationResult::CollectCustomMeasure(std::string name, double value)
{
    if (!_customMeasure.contains(name))
        _customMeasure[name] = EsembledMeasure<>{name, ""};
    _customMeasure[name](value);
}

bool SimulationResult::PrecisionReached()
{
    for (std::string tg : _precisionTargets)
    {
        if (tg == "active_time")
        {
            if (tgt._mean.confidence().precision() > requiredPrecision)
            {
                return false;
            }
        }
        else if (!_acc[tg].Ready())
        {
            return false;
        }
        else if (_customMeasure.contains(tg) && _customMeasure[tg][0].confidence().precision() > requiredPrecision)
        {
            return false;
        }
    }
    return true;
}

auto format_as(ConfidenceHits b)
{
    return fmt::format("Name   Hits   Last\n Throughput {}  {} \n Utilization  {}  {} \n MeanWaits  {}  {}\n "
                       "MeanClients  {}   {}\n ActiveTime {} {}",
                       b.throughput, b.throughput_in, b.utilization, b.utilization_in, b.meanwaits, b.meanWaits_in,
                       b.meanclients, b.meanClients_in, b.activeTimes, b.activeTime_in);
}

bool StationStats::Ready()
{
    for (int i = 0; i < size; i++)
    {
        auto a = (*this)[(MeasureType)i];
        if (a[0].confidence().precision() > SimulationResult::requiredPrecision || a.Count() < 40)
            return false;
    }
    return true;
}

void StationStats::Collect(BaseStation *station)
{
    auto &self = *this;
    self[throughput](station->throughput());
    self[utilization](station->utilization());
    self[meanwait](station->avg_waiting() + station->avg_delay());
    self[meancustomer](station->mean_customer_system());
}

StationStats::StationStats()
{
    _acc[throughput] = EsembledMeasure<>{"throughput", "j/s"}.WithConfidence(SimulationResult::confidence);
    _acc[utilization] = EsembledMeasure<>{"utilization", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meancustomer] = EsembledMeasure<>{"meanclients", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meanwait] = EsembledMeasure<>{"meanwaits", "ms"}.WithConfidence(SimulationResult::confidence);
}

EsembledMeasure<> &StationStats::operator[](StationStats::MeasureType measure)
{
    switch (measure)
    {
    case throughput:
        return _acc[0];
    case utilization:
        return _acc[1];
    case meancustomer:
        return _acc[2];
    case meanwait:
        return _acc[3];
    case size:
        panic("Index out of bounds");
        break;
    }
}

void StationStats::Reset()
{
    for (int i = 0; i < size; i++)
    {
        _acc[i].WithConfidence(SimulationResult::confidence);
        _acc[i].MoveEsemble(1);
        _acc[i][0].Reset();
    }
}
