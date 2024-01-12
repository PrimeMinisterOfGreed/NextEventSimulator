#include "Shell/SimulationShell.hpp"
#include "LogEngine.hpp"
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
}

void SimulationShell::Execute()
{
    bool execend = false;
    while (!execend)
    {
        print("<SimulationShell> ");
        char buffer[256]{};
        std::cin.getline(buffer, 256);
        Command(buffer);
    }
}

void SimulationShell::Command(const char *command)
{
    for (auto c : _cmds)
    {
        if (c.IsCommandFor(command))
        {
            c(command);
            return;
        }
    }
    _logger.Exception("Command {} not found", command);
}

void SimulationShell::RemoveCommand(const char *command)
{
    for (int i = 0; i < _cmds.size(); i++)
    {
        if (strcmp(_cmds[i].command, command) == 0)
        {
            _cmds.erase(_cmds.begin() + i);
        }
    }
}
