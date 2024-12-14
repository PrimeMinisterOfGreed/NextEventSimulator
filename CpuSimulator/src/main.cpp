#include "rngs.hpp"
#include "LogEngine.hpp"
#include "SimulationEnv.hpp"
#include "argparse/argparse.hpp"
#include <cstring>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>




int main(int argc, char **argv)
{
    RandomStream::Global().PlantSeeds(123456799);
    auto manager = SimulationManager::Instance();
    for(auto& trace: LogEngine::Instance()->GetSources()){
        trace->verbosity = 4;
    }
    manager->SetupScenario("Default");
    manager->os->Initialize();
    manager->CollectSamples(-1,true);
    
}
