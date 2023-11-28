#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <string>

enum class LogType
{
    EXCEPTION,
    RESULT,
    INFORMATION,
    TRANSFER,
    DEBUG
};

// use for dynamic format, else use fmt::format
template <typename... Args> std::string makeformat(const char *format, Args... args)
{
    return std::move(std::string(fmt::vformat(std::string(format), fmt::make_format_args(args...))));
}

class ILogEngine
{
  public:
    virtual void Finalize() = 0;
    virtual void Trace(LogType type, std::string message) = 0;
};

struct TraceSource;
class LogEngine : public ILogEngine
{
    friend struct TraceSource;

  private:
    std::stringstream _buffer;
    std::string _logFile;
    static LogEngine *_instance;
    LogEngine()
    {
    }
    LogEngine(std::string logFile) : _logFile{logFile}
    {
    }

  public:
    virtual void Finalize() override;
    virtual void Trace(LogType type, std::string message) override;

    static void CreateInstance(std::string logFile)
    {
        _instance = new LogEngine(logFile);
    }
};

struct TraceSource
{
    std::string sourceName;
    ILogEngine *engine;
    int verbosity;
    TraceSource(std::string sourceName, int verbosity = 4) : sourceName(sourceName), verbosity(verbosity)
    {
        engine = LogEngine::_instance;
    }

    void Trace(LogType type, std::string message)
    {
        if (verbosity >= (int)type)
        {
            engine->Trace(type, makeformat("({}){}", sourceName, message));
        }
    }

    template <typename... Args> void Exception(const char *format, Args... args)
    {
        Trace(LogType::EXCEPTION, makeformat(format, args...));
    }

    template <typename... Args> void Information(const char *format, Args... args)
    {
        Trace(LogType::INFORMATION, makeformat(format, args...));
    }

    template <typename... Args> void Transfer(const char *format, Args... args)
    {
        Trace(LogType::TRANSFER, makeformat(format, args...));
    }

    template <typename... Args> void Result(const char *format, Args... args)
    {
        Trace(LogType::RESULT, makeformat(format, args...));
    }

    template <typename... Args> void Debug(const char *format, Args... args)
    {
        Trace(LogType::DEBUG, makeformat(format, args...));
    }
};