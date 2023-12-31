#include "LogEngine.hpp"
#include "NESssqv2.hpp"
#include "rngs.hpp"
#include <cstdio>
#include <string>
#include <vector>

int endTime = 20000;
int targetTime = 100;

int main(int argc, char *argv[])
{

    RandomStream::Global().PlantSeeds(123456789);
    LogEngine::CreateInstance("simulation.txt");
    TraceSource mainSource{"Main"};

    NESssq simulator{};
    simulator
        //.EndTime(80)
        .Initialize();
    simulator.Execute();
    LogEngine::Instance()->Flush();
}
