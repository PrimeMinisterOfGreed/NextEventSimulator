#pragma once
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include <cstring>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

class SimulationShell
{
  private:
    TraceSource _logger{"Shell", 4};
    friend class Command;
    struct Command
    {
        const char *command;
        std::function<void(SimulationShell *, const char *)> _fnc;
        SimulationShell *_shell;
        template <typename F>
        Command(const char *command, SimulationShell *shell, F &&fnc) : _fnc(fnc), command(command), _shell(shell)
        {
        }
        virtual void operator()(const char *context);
        bool IsCommandFor(const char *context);
    };
    IScheduler *_scheduler;
    ISimulator *_simulator;
    bool _started = false;
    bool _end = false;
    std::thread _executionThread;
    std::vector<Command> _cmds;

  public:
    SimulationShell()
    {
        SetupDefaultCmds();
    }
    void SetControllers(IScheduler *scheduler, ISimulator *simulator)
    {
        _scheduler = scheduler;
        _simulator = simulator;
    }
    void Start();
    void Pause();
    void SetupDefaultCmds();
    void Execute();
    void ShowLog(bool show);
    void ExecuteCommand(const char *command);
    TraceSource *Log()
    {
        return &_logger;
    }
    template <typename F> void AddCommand(const char *command, F &&fnc)
    {
        struct Command cmd(command, this, fnc);
        _cmds.push_back(cmd);
    }
    void RemoveCommand(const char *command)
    {
        std::erase_if(_cmds, [command](Command &cmd) { return strcmp(cmd.command, command) == 0; });
    }
    void ClearCommands()
    {
        _cmds.clear();
    }
    template <typename T> std::stringstream& Parse(std::stringstream& stream, T*val)
    {
        return stream;
    }
};