
#include "Core.hpp"
#include "Shell/SimulationShell.hpp"
#include "SimulationEnv.hpp"
#include <cstdlib>
#include <exception>
#include <fmt/core.h>
#include <optional>


int main(int argc, char **argv)
{
    LogEngine::CreateInstance("simulation.txt");
    RandomStream::Global().PlantSeeds(123456789);
    SimulationShell &shell = SimulationShell::Instance();
    SimulationManager::Instance().SetupShell(&shell);
    shell.Execute();
}
