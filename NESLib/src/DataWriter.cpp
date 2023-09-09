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

void DataWriter::WriteLine(std::string data)
{
    using namespace std::filesystem;
    _lines[_currentIndex++] = data;
    if (_currentIndex == 1000)
    {
        Flush();
    }
}

void DataWriter::Flush()
{
    std::ofstream file{};
    auto path = std::filesystem::path{"data.csv"};
    if (!exists(path))
    {
        file.open("data.csv", std::ios_base::out);
        file << fmt::format("{}\n", header);
    }
    else
    {
        file.open("data.csv", std::ios_base::app);
    }
    for (int i = 0; i < _currentIndex; i++)
    {
        file << fmt::format("{}\n", _lines[i]);
        _lines[i].clear();
    }

    _currentIndex = 0;
}
