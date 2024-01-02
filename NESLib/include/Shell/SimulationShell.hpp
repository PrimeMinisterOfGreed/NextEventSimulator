#pragma once
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include <cstring>
#include <functional>
#include <thread>
#include <vector>

class SimulationShell
{
  private:
    TraceSource _logger{"Shell"};
    struct Command
    {
        const char *command;
        std::function<void(SimulationShell *, const char *)> _fnc;
        SimulationShell *_shell;
        template <typename F>
        Command(const char *command, SimulationShell *shell, F &&fnc) : _fnc(fnc), command(command), _shell(shell)
        {
        }
        virtual void operator()(const char *context)
        {
            char buffer[256]{};
            memcpy(buffer, context + strlen(command),
                   strlen(context) > 255 ? 255 - strlen(command) : strlen(context) - strlen(command));
            _fnc(_shell, buffer);
        }

        bool IsCommandFor(const char *context)
        {
            auto size = strlen(command);
            for (int i = 0; i < size; i++)
            {
                if (context[i] != command[i])
                    return false;
            }
            return true;
        }
    };
    IScheduler *_scheduler;
    ISimulator *_simulator;
    bool _started = false;
    bool _end = false;
    std::thread _executionThread;
    std::vector<Command> _cmds;

  public:
    SimulationShell(IScheduler *scheduler, ISimulator *simulator) : _scheduler(scheduler), _simulator(simulator)
    {
        SetupDefaultCmds();
    }

    void Start();
    void Pause();
    void SetupDefaultCmds();
    void Execute();
    void ShowLog(bool show);
    void Command(const char *command);

    template <typename F> void AddCommand(const char *command, F &&fnc)
    {
        struct Command cmd(command, this, fnc);
        _cmds.push_back(cmd);
    }
    void RemoveCommand(const char *command);
    void ClearCommands()
    {
        _cmds.clear();
    }
};