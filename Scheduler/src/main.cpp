
#include "Shell/SimulationShell.hpp"
#include "SimulationEnv.hpp"

int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    RandomStream::Global().PlantSeeds(123456789);
    SimulationShell shell{};
    SimulationManager::Instance().SetupShell(&shell);
    shell.Execute();
}
