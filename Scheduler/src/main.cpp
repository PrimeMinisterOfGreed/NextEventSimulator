
#include "SimulationEnv.hpp"

int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    RandomStream::Global().PlantSeeds(123456789);
    SetupEnvironment();
    shell.Execute();
}
