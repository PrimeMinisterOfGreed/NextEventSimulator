#include "LogEngine.hpp"
#include "NESssqv2.hpp"
#include "rngs.hpp"
#include <boost/program_options.hpp>
#include <string>
#include <vector>

boost::program_options::variables_map optionsMap = *new boost::program_options::variables_map();
int endTime = 20000;
int targetTime = 100;

int main(int argc, char *argv[])
{
    using namespace boost::program_options;

    // Declare the supported command line options
    options_description desc("Allowed options");

    desc.add_options()("-help", "produce help message")("-seed", value<int>()->default_value(123456789),
                                                        "seed the random number generator")(
        "-file", value<std::string>(), "if specified uses the trace driven simulator")(
        "-output", value<std::string>()->default_value("log.txt"),
        "if specified write the log to a specific file instead of default log.txt")(
        "-probe", value<std::vector<int>>(), "if specified decides when launch the probe event (or multiple times)")(
        "-end", value<int>()->default_value(20000),
        "if specified writes the final event to a certain value instead of default (20000)")(
        "-verbosity", value<int>()->default_value(1), "set the verbosity level (default 1)");

    variables_map vm;
    try
    {
        store(command_line_parser(argc, argv).options(desc).run(), vm);
        optionsMap = vm;
    }
    catch (boost::program_options::error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    NESssq *simulator;
    RandomStream::Global().PlantSeeds(vm.at("-seed").as<int>());
    endTime = vm.at("-end").as<int>();
    ConsoleLogEngine::CreateInstance(vm.at("-verbosity").as<int>(), vm.at("-output").as<std::string>());
    if (vm.count("-file"))
        simulator = new NESssq(new TraceDrivenDataProvider(vm.at("-file").as<std::string>(), true),
                               ConsoleLogEngine::Instance());
    else
    {
        simulator =
            new NESssq(new DoubleStreamNegExpRandomDataProvider(endTime, 0.14, 0.10), ConsoleLogEngine::Instance());
    }
    simulator->Execute();
    simulator->Report();
}
