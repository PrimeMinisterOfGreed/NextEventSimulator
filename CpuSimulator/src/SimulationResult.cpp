#include "SimulationResult.hpp"
#include "Core.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include "TaggedCustomer.hpp"
#include "SystemParameters.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <vector>




SimulationResult::SimulationResult() : _logger("SimulationResults", 1)
{

}



void SimulationResult::Reset()
{
    for (auto &v : _acc)
    {
        v.second.Reset();
    }
    tgt._mean.Reset();
    tgt._acc.Reset();
}

void SimulationResult::Collect(const BaseStation &station)
{
        _acc[station.Name()].Collect(station);
}



bool SimulationResult::PrecisionReached()
{
    for (std::string tg : _precisionTargets)
    {
        if (tg == "ActiveTime" && tgt._mean.confidence().precision() > requiredPrecision)
        {
            return false;
        }
        else if (!_acc[tg].Ready())
        {

            return false;
        }
    }
    return true;
}


std::string format_station_measures(std::string name, StationStats &acc)
{

    std::string result = "";
    for (int i = StationStats::meancustomer; i < StationStats::MeasureType::size; i++)
    {
 
        result += fmt::format("{:5f}\n", acc[(StationStats::MeasureType)i]);
    }
    return result;
}

std::string get_active_time(const TaggedCustomer &tgt)
{

    return fmt::format("{}", tgt._mean);
}

std::string format_measure_output(std::string stationName, std::string measureOutput)
{
    std::string log = "";
    log += "################################\n";
    log += fmt::format("Station_Name:{}\n", stationName);
    log += measureOutput;
    log += "################################\n";
    return log;
}

void SimulationResult::LogResult(std::string name)
{
    std::string log = "";
    if (name.compare("ALL") == 0)
    {
        for (auto s : _acc)
        {
            log += format_measure_output(s.first, format_station_measures(name,  _acc[s.first]));
        }
        log += get_active_time( tgt);
    }
    else if (name == "ACTIVETIME" || name == "ActiveTime")
    {
        log = get_active_time(tgt);
    }
    else
    {
        log += format_measure_output(name, format_station_measures(name,  _acc[name]));
    }
    fmt::print("{}",log);
}


std::string format_as(StationStats stats)
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

void StationStats::Collect(const BaseStation &station)
{

    // this[throughput](station->completions(), station->clock() - self[throughput].times());
    // this[utilization](station->busyTime(), station->clock() - self[utilization].times());
    _acc[meanwait](station.areaN(), station.completions());
    _acc[meancustomer](station.areaN(), station.observation());
}

StationStats::StationStats()
{
    _acc[throughput] = CovariatedMeasure{"throughput", "j/s"}.WithConfidence(SimulationResult::confidence);
    _acc[utilization] = CovariatedMeasure{"utilization", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meancustomer] = CovariatedMeasure{"meanclients", ""}.WithConfidence(SimulationResult::confidence);
    _acc[meanwait] = CovariatedMeasure{"meanwaits", "ms"}.WithConfidence(SimulationResult::confidence);
}

#pragma clang diagnostic ignored "-Wreturn-type"
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
