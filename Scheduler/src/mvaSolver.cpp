#include <Mva.hpp>
#include <fmt/core.h>
#include <string>
#include <vector>

int main()
{

    std::vector<double> visits = {1, 250, 2.5, 16.25, 6.25};
    std::vector<double> stimes = {5000, 210, 2.7, 40, 180};
    std::vector<StationType> types = {D, I, I, I, I};
    auto result = MVALID(visits, stimes, types, 30);
    std::string toString = "";
}