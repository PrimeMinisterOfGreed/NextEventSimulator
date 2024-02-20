#include "SimulationResult.hpp"
#include "Core.hpp"
#include "Measure.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include <cstring>
#include <fmt/core.h>
#include <fmt/format.h>
#include <sstream>
#include <string>
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

        _confidenceHits[station].Accumulate(ref[StationStats::throughput].confidence().isInTval(t_throughput),
                                            ref[StationStats::utilization].confidence().isInTval(t_utilization),
                                            ref[StationStats::meancustomer].confidence().isInTval(t_meanclients),
                                            ref[StationStats::meanwait].confidence().isInTval(t_meanwait),
                                            tgt._mean.confidence().isInTval(t_activeTime));
        seeds.push_back(seed);
    }
}

SimulationResult::SimulationResult()
{
}

void SimulationResult::AddShellCommands(SimulationShell *shell)
{
    shell->AddCommand("lsimresults", [this](SimulationShell *shell, const char *context) { LogSimResults(); });
    shell->AddCommand("ltgtstats", [this](SimulationShell *s, auto ctx) {
        s->Log()->Result("{},Expected:{}", tgt._mean, mva.ActiveTimes()[SystemParameters::Parameters().numclients]);
    });

    shell->AddCommand("lmeasures", [this](SimulationShell *shell, const char *ctx) {
        char buffer[36]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        if (strlen(buffer) == 0)
        {
            LogResult();
        }
        else
        {
            LogResult(std::string{buffer});
        }
    });
    shell->AddCommand("queryMeasure", [this](SimulationShell *shell, const char *ctx) {
        char buffer[36]{};
        std::stringstream stream{ctx};
        stream >> buffer;
        auto station = std::string(buffer);
        stream >> buffer;
        if (station == "ActiveTime")
        {
            fmt::println("ActiveTime;{:csv};{}",tgt._mean,mva.ActiveTimes()[SystemParameters::Parameters().numclients]);
            return;
        }
        auto measure = std::string(buffer);
        auto s = _acc[station];
        auto &acc = measure == "N" ? s._acc[s.meancustomer] : s._acc[s.meanwait];
        auto expected = mva.ExpectedForAccumulator(station, &acc);
        fmt::println("{};{:csv};{}", station, acc, expected);
    });
    shell->AddCommand("reset_measures", [this](auto s, auto ctx) { Reset(); });
}

void SimulationResult::Reset()
{
    for (auto &v : _acc)
    {
        v.second.Reset();
    }
    _activeTime.Reset();
    tgt._mean.Reset();
    tgt._acc.Reset();
}

void SimulationResult::Collect(BaseStation *station)
{
    _acc[station->Name()].Collect(station);
}

bool SimulationResult::PrecisionReached()
{
    auto logger = SimulationShell::Instance().Log();
    for (std::string tg : _precisionTargets)
    {
        if (tg == "ActiveTime" && tgt._mean.confidence().precision() > requiredPrecision)
        {
            logger->Information("Target:{} not reached precision", tg);
            return false;
        }
        else if (!_acc[tg].Ready())
        {
            logger->Information("Target:{} not reached precision", tg);

            return false;
        }
    }
    logger->Information("All target reached required precision {} ", requiredPrecision);
    return true;
}

void SimulationResult::LogSimResults()
{
    auto logger = SimulationShell::Instance().Log();

    for (auto s : mva.Stations())
    {
        logger->Result("Station:{}\n{}", s, _confidenceHits[s]);
    }
}

void SimulationResult::LogResult(std::string name)
{
    if (name == "ALL")
    {
        for (auto s : _acc)
        {
            std::string result = "";
            for (int i = StationStats::meancustomer; i < StationStats::MeasureType::size; i++)
            {
                auto expected = mva.ExpectedForAccumulator(s.first, &s.second[(StationStats::MeasureType)i]);
                result += fmt::format("{}, Expected Value:{}\n", s.second[(StationStats::MeasureType)i], expected);
            }
            SimulationShell::Instance().Log()->Result("Station:{}\n{}", s.first, result);
        }

        SimulationShell::Instance().Log()->Result("{},Expected:{}", tgt._mean,
                                                  mva.ActiveTimes()[SystemParameters::Parameters().numclients]);
    }
    else if (name == "ActiveTime")
    {
        SimulationShell::Instance().Log()->Result("{},Expected:{}", tgt._mean,
                                                  mva.ActiveTimes()[SystemParameters::Parameters().numclients]);
    }
    else
    {
        auto acc = _acc[name];
        std::string result = "";
        for (int i = StationStats::meancustomer; i < StationStats::MeasureType::size; i++)
        {
            auto expected = mva.ExpectedForAccumulator(name, &acc[(StationStats::MeasureType)i]);
            result += fmt::format("{}, Expected Value:{}\n", acc[(StationStats::MeasureType)i], expected);
        }
        SimulationShell::Instance().Log()->Result("Station:{}\n{}", name, result);
    }
}

auto format_as(ConfidenceHits b)
{
    return fmt::format("Name   Hits   Last\n Throughput {}  {} \n Utilization  {}  {} \n MeanWaits  {}  {}\n "
                       "MeanClients  {}   {}\n ActiveTime {} {}",
                       b.throughput, b.throughput_in, b.utilization, b.utilization_in, b.meanwaits, b.meanWaits_in,
                       b.meanclients, b.meanClients_in, b.activeTimes, b.activeTime_in);
}

auto format_as(StationStats stats)
{
    std::string result = "";
    for (int i = 0; i < StationStats::size; i++)
    {
        result += fmt::format("{}", stats[(StationStats::MeasureType)i]);
    }
    return result;
}

bool StationStats::Ready()
{
    for (int i = 0; i < size; i++)
    {
        auto a = (*this)[(MeasureType)i];
        if (a.confidence().precision() > SimulationResult::requiredPrecision || a.Count() < 2)
        {
            return false;
        }
    }
    return true;
}

void StationStats::Collect(BaseStation *station)
{
    auto &self = *this;
    self[throughput](station->completions(), station->clock() - self[throughput].times());
    self[utilization](station->busyTime(), station->clock() - self[utilization].times());
    self[meanwait](station->areaN(), station->completions());
    self[meancustomer](station->areaN(), station->clock() - self[meancustomer].times());
}

StationStats::StationStats()
{
    _acc[throughput] = CovariatedMeasure{"throughput", "j/s"}.WithConfidence(SimulationResult::confidence);
    _acc[utilization] = CovariatedMeasure{"utilization", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meancustomer] = CovariatedMeasure{"meanclients", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meanwait] = CovariatedMeasure{"meanwaits", "ms"}.WithConfidence(SimulationResult::confidence);
}

CovariatedMeasure &StationStats::operator[](StationStats::MeasureType measure)
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
        _acc[i].Reset();
    }
}
