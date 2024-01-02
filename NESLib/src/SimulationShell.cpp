#include "Shell/SimulationShell.hpp"
#include "LogEngine.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <iostream>
#include <sched.h>
#include <sstream>
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
    AddCommand("verbosity", [](auto s, const char *context) {
        char buffer[64]{};
        std::stringstream buf{context};
        buf.get(buffer, ' ');
        int verbosity = atoi(buffer);
        for (auto s : LogEngine::Instance()->GetSources())
        {
            s->verbosity = verbosity;
        }
    });

    AddCommand("start", [](SimulationShell *s, auto ctx) { s->Start(); });

    AddCommand("pause", [](SimulationShell *shell, auto ctx) { shell->Pause(); });

    AddCommand("init", [](SimulationShell *shell, auto ctx) { shell->_simulator->Initialize(); });

    AddCommand("clock", [this](SimulationShell *shell, auto ctx) {
        _logger.Result("Clock is {}", shell->_scheduler->GetClock());
    });

    AddCommand("ne", [](SimulationShell *shell, auto ctx) { shell->_simulator->Execute(); });
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
