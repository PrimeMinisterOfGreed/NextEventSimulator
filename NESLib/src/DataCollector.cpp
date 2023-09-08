#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "LogEngine.hpp"
#include "rvms.h"
#include <fmt/format.h>
#include <string>

std::vector<Measure<double>> DataCollector::GetAccumulators() const
{
    return _measures;
}

void DataCollector::AddMeasure(Measure<double> measure)
{
    _measures.push_back(measure);
}

std::string DataCollector::Header()
{
    std::string head = "";
    head += "TimeStamp;StationName;";
    for (auto measure : _measures)
    {
        head += makeformat("{};", measure.Name());
        head += makeformat("meanOf{};", measure.Name());
        head += makeformat("varianceOf{};", measure.Name());
        head += makeformat("lowerBoundOf{};", measure.Name());
        head += makeformat("upperBoundOf{};", measure.Name());
    }
    return head;
}

std::string DataCollector::Csv()
{
    std::string csv = "";
    csv += makeformat("{};{};", lastTimeStamp, _stationName);
    for (auto measure : _measures)
    {
        csv += makeformat("{};", measure.LastValue());
        csv += makeformat("{};", measure.mean());
        csv += makeformat("{};", measure.variance());
        csv += makeformat("{};", measure.confidence().first);
        csv += makeformat("{};", measure.confidence().second);
    }
    return csv;
}

DataCollector::DataCollector(std::string stationName) : _stationName(stationName)
{
}
