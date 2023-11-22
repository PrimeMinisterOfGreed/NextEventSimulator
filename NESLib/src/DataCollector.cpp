#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "Usings.hpp"
#include "rvms.h"
#include <fmt/format.h>
#include <string>

std::vector<sptr<Measure<double>>> DataCollector::GetAccumulators() const
{
    return _measures;
}

void DataCollector::AddMeasure(sptr<Measure<double>> measure)
{
    _measures.push_back(measure);
}

std::string DataCollector::Header()
{
    std::string head = "";
    head += "TimeStamp;StationName;";
    for (auto measure : _measures)
    {
        head += makeformat("{};", measure->Heading());
    }
    head.erase(head.end() - 1);
    return head;
}

std::string DataCollector::Csv()
{
    std::string csv = "";
    csv += makeformat("{};{};", lastTimeStamp, _stationName);
    for (auto measure : _measures)
    {
        csv += makeformat("{};", measure->Csv());
    }
    csv.erase(csv.end() - 1);
    return csv;
}

DataCollector::DataCollector(std::string stationName) : _stationName(stationName)
{
}

DataCollector::~DataCollector()
{
}
