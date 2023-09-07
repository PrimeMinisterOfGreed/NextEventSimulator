#include "DataWriter.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ios>

DataWriter &DataWriter::Instance()
{
    static DataWriter instance{};
    return instance;
}

void DataWriter::Write(std::string data)
{
    using namespace std::filesystem;
    _lines[_currentIndex++] = data;
    if (_currentIndex == 1000)
    {
        Flush();
        _currentIndex = 0;
    }
}

void DataWriter::Flush()
{
    std::ofstream file{};
    auto path = std::filesystem::path{"data.csv"};
    if (exists(path))
    {
        file.open("data.csv", std::ios_base::out);
        file << fmt::format("{}\n", header);
    }
    else
    {
        file.open("data.csv", std::ios_base::app);
    }
    for (auto line : _lines)
    {
        file << fmt::format("{}\n", line);
    }
}
