#include "Chains/DTMC.hpp"
#include "Collections/Matrix.hpp"
#include <Mva.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

std::string MVAToString(Matrix<double> result, std::string resultName)
{
    auto res = fmt::format("########{}######\n", resultName); 
    for (int i = 0; i < result.Rows(); i++)
    {
        res += fmt::format("S_{}_{}=[{}]\n", i,resultName, fmt::join(result.Row(i),","));
    }
    return res;
}

int main()
{
    Matrix<double> q{{
        {0,1,0,0,0},
        {0,0,1,0,0},
        {0.004,0.006,0.9,0.065,0.025},
        {0,0,1,0,0},
        {0,0,1,0,0}
    }};
    std::vector<double> visits = powerMatrixMethod(q,10000);
    for(int i = 1; i < visits.size(); i++){
        visits[i] = visits[i]*(1/visits[0]);
    }
    //std::vector<double> visits = {1, 250, 2.5, 16.25, 6.25};
    std::vector<double> stimes = {5000, 210, 2.7, 40, 180};
    std::vector<StationType> types = {D, I, I, I, I};
    auto result = MVALID(visits, stimes, types, 30);
    std::string resultstr = "";
    resultstr += fmt::format("{}",MVAToString(result.meanClients, "meanClients"));
    resultstr += fmt::format("{}",MVAToString(result.meanWaits, "meanWaits"));
    resultstr += fmt::format("{}",MVAToString(result.throughputs, "throughputs"));
    resultstr += fmt::format("{}",MVAToString(result.utilizations, "utilizations"));
    fmt::print("{}",resultstr);
    std::fstream file{"mva.py",std::ios_base::out};
    file.write(resultstr.c_str(), resultstr.size());
}