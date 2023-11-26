#include "DataWriter.hpp"
#include "LogEngine.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ios>

void DataWriter::WriteFile(const DataFile &dataFile)
{
    std::fstream stream{dataFile.fileName, std::ios::out | std::ios::app};
    if (stream.tellp() == 0)
    {
        auto ref = dataFile.collectors.at(0);
        stream.write(ref->Header().c_str(), ref->Header().size());
    }
    for (auto &ref : dataFile.collectors)
    {
        auto res = ref->Csv();
        stream.write(res.c_str(), res.size());
    }
}

void DataWriter::Register(DataCollector *collector)
{
    if (_dataFiles.size() == 0)
    {
        auto data = DataFile{.fileName = "data.csv"};
        _dataFiles.push_back(data);
    }
    for (auto &category : _dataFiles)
    {
        if (category.isInCategory(*collector))
        {
            category.collectors.push_back(collector);
            return;
        }
    }
    auto data = DataFile{.fileName = makeformat("data{}.csv", _dataFiles.size())};
    data.collectors.push_back(collector);
    _dataFiles.push_back(data);
}

void DataWriter::SnapShot()
{
    for (auto &file : _dataFiles)
    {
        WriteFile(file);
    }
}
