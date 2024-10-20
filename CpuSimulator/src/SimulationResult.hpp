#pragma once
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "MvaSolver.hpp"
#include "Scheduler.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "Strategies/TaggedCustomer.hpp"
#include <fmt/core.h>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

struct ConfidenceHits
{
    int throughput = 0;
    int utilization = 0;
    int meanwaits = 0;
    int meanclients = 0;
    int activeTimes = 0;
    bool throughput_in = false;
    bool utilization_in = false;
    bool meanClients_in = false;
    bool meanWaits_in = false;
    bool activeTime_in = false;
    void Accumulate(bool x_in, bool u_in, bool n_in, bool w_in, bool activeTime_in);
};

std::string format_as(ConfidenceHits b);

struct StationStats
{
    std::string station_name{};
    CovariatedMeasure _acc[4];
    void Collect(const BaseStation& station);
    StationStats();
    enum MeasureType
    {
        throughput,
        utilization,
        meancustomer,
        meanwait,
        size
    };

    CovariatedMeasure &operator[](StationStats::MeasureType measure);
    void Reset();
    bool Ready();
};


std::string format_as(StationStats stats);

struct SimulationResult
{
    static inline double requiredPrecision = 0.05;
    static inline double confidence = 0.90;
    std::map<std::string, StationStats> _acc;
    Accumulator<> _activeTime{"activeTime", "ms"};
    std::vector<std::string> _precisionTargets = {"CPU", "IO1", "IO2", "ActiveTime"};
    MVASolver mva{};
    std::map<std::string, ConfidenceHits> _confidenceHits{};
    std::vector<int> seeds;
    TaggedCustomer tgt{};
    TraceSource _logger;

    void CollectResult(int seed);
    SimulationResult();
    void AddShellCommands(SimulationShell *shell);
    void Reset();
    void Collect(const BaseStation &station);
    void CollectCustomMeasure(std::string name, double value, double clock);
    void CollectActiveTime(double value);
    bool PrecisionReached();
    void LogSimResults();
    bool IsTransitoryPeriod();
    void AddPrecisionTarget(std::string name)
    {
        _precisionTargets.push_back(name);
    }
    void LogResult(std::string name = "ALL");
};
