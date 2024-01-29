#include "MvaSolver.hpp"
#include "Collections/Matrix.hpp"
#include "Core.hpp"
#include "Enums.hpp"
#include "Measure.hpp"
#include "SystemParameters.hpp"
#include <Mva.hpp>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

static Matrix<double> q{
    {{0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0.004, 0.006, 0.9, 0.065, 0.025}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}}};

const std::vector<double> stimes{5000, 210, 2.7, 40, 180};
// delay,swap_in,cpu,io1,io2
const std::vector<StationType> types{D, I, I, I, I};

const std::vector<std::string> stations{"Delay", "SWAP_IN", "CPU", "IO1", "IO2"};

std::vector<double> visits;
MVAResult preloadResult{(int)stimes.size(), 30};

std::string MVAToString(Matrix<double> result, std::string resultName)
{
    auto res = fmt::format("########{}######\n", resultName);
    for (int i = 0; i < result.Rows(); i++)
    {
        res += fmt::format("S_{}_{}=[{}]\n", i, resultName, fmt::join(result.Row(i), ","));
    }
    return res;
}

std::string MVAToCsv(Matrix<double> result, std::string resultName)
{
    using namespace fmt;
    std::string header = "N;";
    for (int i = 0; i < result.Rows(); i++)
        header += format("S{};", i);
    header.erase(header.size() - 1);
    std::string res = format("{}\n", header);
    std::string line = "";
    for (int j = 0; j < result.Columns(); j++)
    {
        res += format("{};{}\n", j, join(result.Col(j), ";"));
    }
    return res;
}

void MVAToFile(Matrix<double> result, std::string resultName)
{
    using namespace fmt;
    auto csv = MVAToCsv(result, resultName);
    print("{}\n{}", resultName, csv);
    std::fstream file{format("{}.csv", resultName), std::ios_base::out};
    file.write(csv.c_str(), csv.size());
}

void PrintMVA()
{
    MVAToFile(preloadResult.meanClients, "meanClients");
    MVAToFile(preloadResult.meanWaits, "meanWaits");
    MVAToFile(preloadResult.throughputs, "throughputs");
    MVAToFile(preloadResult.utilizations, "utilizations");
}

void DoMva(Matrix<double> transition)
{
    auto calc = RouteToVisit(transition);
    visits = calc;
    preloadResult = MVALID(std::vector{visits}, std::vector{stimes}, std::vector{types}, 30);
}

void MVASolver::PreloadModel()
{
    DoMva(q);
    inited = true;
}

std::vector<std::string> MVASolver::Stations()
{
    return stations;
}

int peekStation(std::string stationName)
{
    for (int i = 0; i < stations.size(); i++)
        if (stationName == stations[i])
            return i;
    core_assert(false, "Station {} not found", stationName);
    return -1;
}

std::vector<double> MVASolver::Throughputs(std::string stationName)
{
    return preloadResult.throughputs.Row(peekStation(stationName));
}
std::vector<double> MVASolver::Utilizations(std::string stationName)
{
    return preloadResult.utilizations.Row(peekStation(stationName));
}
std::vector<double> MVASolver::MeanClients(std::string stationName)
{
    return preloadResult.meanClients.Row(peekStation(stationName));
}
std::vector<double> MVASolver::MeanWaits(std::string stationName)
{
    return preloadResult.meanWaits.Row(peekStation(stationName));
}

std::vector<double> MVASolver::ActiveTimes()
{
    static std::vector<double> times{};
    if (times.size() > 0)
        return times;
    auto mat = preloadResult.meanClients;
    auto t = preloadResult.throughputs;
    for (int k = 0; k < preloadResult.meanWaits.Columns(); k++)
    {
        times.push_back((mat(2, k) + mat(3, k) + mat(4, k)) / t(1, k));
    }
    return times;
}

double MVASolver::ExpectedForAccumulator(std::string stationName, const BaseMeasure *acc)
{
    int n = SystemParameters::Parameters().numclients;
    if (!inited)
        PreloadModel();
    if (std::find(stations.begin(), stations.end(), stationName) == stations.end())
        return 0.0;
    if (acc->Name() == "throughput")
    {
        return Throughputs(stationName)[n];
    }
    else if (acc->Name() == "utilization")
    {
        return Utilizations(stationName)[n];
    }
    else if (acc->Name() == "meanclients")
    {
        return MeanClients(stationName)[n];
    }
    else if (acc->Name() == "meanwaits")
    {
        return MeanWaits(stationName)[n];
    }
    else
        return ActiveTimes()[n];
}

#ifdef USE_MAIN
int main()
{

    DoMva(q);
    PrintMVA();
}
#endif
