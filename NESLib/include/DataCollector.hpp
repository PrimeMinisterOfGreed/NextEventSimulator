#pragma once

#include "Measure.hpp"
#include <map>
#include <string>
#include <vector>

using Interval = std::pair<double, double>;

class Station;
class DataCollector
{
    friend class Station;

  private:
    int _samples = 0;

  public:
    int getSamples() const;

  private:
    std::string _stationName;
    std::vector<Measure<double> *> _measures;
    double lastTimeStamp;

  public:
    std::vector<Measure<double> *> GetAccumulators() const;
    Measure<double> &operator[](int index)
    {
        return *_measures[index];
    }
    inline double TimeStamp() const
    {
        return lastTimeStamp;
    }
    void AddMeasure(Measure<double> *measure);
    std::string Header();
    std::string Csv();
    DataCollector(std::string stationName);
    ~DataCollector();
};
