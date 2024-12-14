#include "LogEngine.hpp"
#include "Shell/SimulationShell.hpp"
#include "SimulationEnv.hpp"
#include "argparse/argparse.hpp"
#include <cstring>
#include <fmt/core.h>
#include <sstream>
#include <rngs.hpp>
#include <streambuf>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    argparse::ArgumentParser parser("Scheduler");
    bool interactive = false;
    parser.add_argument("it", "--interactive")
        .implicit_value("dummy")
        .help("Execute in interactive mode")
        .action([&interactive](auto val) { interactive = true; });
    std::vector<std::string> commands{};
    parser.add_argument("prg", "--program")
        .help("Execute a set of specific commands separated by ;")
        .action([&commands](const std::string &value) {
            std::stringstream stream{value};
            char buffer[2048]{};
            while (!stream.eof())
            {
                stream.getline(buffer, 2048, ';');
                commands.push_back(buffer);
                memset(buffer, 0, 2048);
            }
        });

    parser.add_argument("nl", "--no-log").implicit_value("dummy").help("Deactivates logs").action([](auto val) {
        LogEngine::Instance()->PrintStdout(false);
    });
    parser.parse_args(argc, argv);
    RandomStream::Global().PlantSeeds(123456789);
    SimulationShell &shell = SimulationShell::Instance();
    //SimulationManager::Instance().SetupShell(&shell);
    if (interactive)
        shell.Interactive();
    else if (commands.size() > 0)
    {
        for (auto &cmd : commands)
        {
            shell.ExecuteCommand(cmd.c_str());
        }
    }
}