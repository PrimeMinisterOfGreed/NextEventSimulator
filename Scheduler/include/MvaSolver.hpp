#pragma once

#include <map>
#include <string>
#include <vector>
struct MVASolver
{
    bool inited = false;
    void PreloadModel();
    std::vector<std::string> Stations();
    std::vector<double> Throughputs(std::string stationName);
    std::vector<double> Utilizations(std::string stationName);
    std::vector<double> MeanClients(std::string stationName);
    std::vector<double> MeanWaits(std::string stationName);
    std::vector<double> ActiveTimes();
};
