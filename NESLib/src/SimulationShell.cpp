#include "Shell/SimulationShell.hpp"
#include "LogEngine.hpp"
#include "rngs.hpp"
#include <algorithm>
#include <any>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <thread>
#include <tuple>
#include <variant>

using namespace fmt;

void SimulationShell::Start()
{
    if (_started)
        return;
    _executionThread = std::thread{[this]() {
        _end = false;
        _started = true;
        while (!_end)
        {
            _simulator->Execute();
        }
    }};

    _executionThread.detach();
}

void SimulationShell::Pause()
{
    _end = true;
    _started = false;
}

void SimulationShell::SetupDefaultCmds()
{
    AddCommand("verbosity", [](SimulationShell *shell, const char *context) {
        char buffer[64]{};
        std::stringstream buf{context};
        buf.get(buffer, sizeof(buffer), ' ');
        if (isdigit(buffer[0]))
        {
            int verbosity = atoi(buffer);
            for (auto s : LogEngine::Instance()->GetSources())
            {
                s->verbosity = verbosity;
            }
            shell->Log()->Information("Setted all sources to {} verbosity", verbosity);
        }
        else
        {
            std::string srcName{buffer};
            if (srcName == "list")
            {
                for (auto s : LogEngine::Instance()->GetSources())
                {
                    shell->Log()->Information("SourceName:{}, Verbosity:{}", s->sourceName, s->verbosity);
                }
                return;
            }
            buf.ignore();
            buf.get(buffer, sizeof(buffer), ' ');
            int verbosity = atoi(buffer);
            for (auto s : LogEngine::Instance()->GetSources())
            {
                if (s->sourceName == srcName)
                {
                    s->verbosity = verbosity;
                    shell->Log()->Information("Setted {} to verbosity {}", s->sourceName, verbosity);
                    return;
                }
            }
            shell->Log()->Exception("SourceName {} not found", srcName);
        }
    });

    AddCommand("start", [](SimulationShell *s, auto ctx) { s->Start(); });

    AddCommand("pause", [](SimulationShell *shell, auto ctx) { shell->Pause(); });

    AddCommand("init", [](SimulationShell *shell, auto ctx) { shell->_simulator->Initialize(); });

    AddCommand("clock", [this](SimulationShell *shell, auto ctx) {
        _logger.Result("Clock is {}", shell->_scheduler->GetClock());
    });

    AddCommand("ne", [](SimulationShell *shell, auto ctx) {
        std::stringstream stream{ctx};
        char arg[12];
        stream.get(arg, 12, ' ');
        int value = atoi(arg);
        if (value == 0)
            value = 1;
        for (int i = 0; i < value; i++)
            shell->_simulator->Execute();
    });

    AddCommand("help", [this](auto s, auto ctx) {
        fmt::print("List of available commands:");
        for (auto c : _cmds)
        {
            fmt::println("Command :{}", c.command);
        }
    });

    AddCommand("seed", [this](auto s, const char *ctx) {
        char buffer[36]{};
        std::istringstream stream{ctx};
        stream >> buffer;
        if(strlen(buffer) == 0){
            _logger.Exception("Seed must be called with argument");
            return;
        }
        int seed = atoi(buffer);
        RandomStream::Global().PlantSeeds(seed);
    });
}

void SimulationShell::Execute()
{
    bool execend = false;
    while (!execend)
    {
        print("<SimulationShell> ");
        char buffer[256]{};
        std::cin.getline(buffer, 256);
        ExecuteCommand(buffer);
    }
}

void SimulationShell::ExecuteCommand(const char *command)
{
    char buffer[36]{};
    char args[128]{};
    std::istringstream ctx{command};
    ctx >> buffer;
    ctx.get(args, 128);
    for (auto c : _cmds)
    {
        if (c.IsCommandFor(buffer))
        {
            c(args);
            return;
        }
    }
    _logger.Exception("Command {} not found", command);
}

bool SimulationShell::Command::IsCommandFor(const char *context)
{
    char buffer[36]{};
    std::stringstream ctx{context};
    ctx.get(buffer, 36, ' ');
    auto size = strlen(command);
    if (strlen(buffer) != size)
        return false;
    return strcmp(buffer, command) == 0;
}

void SimulationShell::Command::operator()(const char *context)
{
    char buffer[256]{};
    std::stringstream stream{context};
    stream.ignore();
    stream.read(buffer, 256);
    _fnc(_shell, buffer);
}