#include "LogEngine.hpp"
#include <cstdio>
#include <fmt/args.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>
#include <string>

LogEngine *LogEngine::_instance = nullptr;

constexpr std::string LogTypeToString(LogType logType)
{
    switch (logType)
    {
    case LogType::EXCEPTION:
        return std::move(std::string("[Exception]"));
    case LogType::RESULT:
        return std::move(std::string("[Result]"));
    case LogType::INFORMATION:
        return std::move(std::string("[Information]"));
    case LogType::TRANSFER:
        return std::move(std::string("[Transfer]"));
    case LogType::DEBUG:
        return std::move(std::string("[Debug]"));
    }
    return "";
}

constexpr fmt::color LogTypeToColor(LogType type)
{
    using fmt::color;
    switch (type)
    {
    case LogType::EXCEPTION:
        return fmt::color::red;
    case LogType::RESULT:
        return fmt::v8::color::green;
    case LogType::INFORMATION:
        return fmt::v8::color::blue;
    case LogType::TRANSFER:
        return fmt::v8::color::aqua;
    case LogType::DEBUG:
        return fmt::v8::color::yellow;
    }
    return fmt::v8::color::white;
}

void writeBuffer(std::istream *buffer, std::ostream *save)
{
    using namespace std;
    std::string line;
    while (std::getline(*buffer, line))
    {
        char statBuf[line.size() + 1];
        sprintf(statBuf, "%s\n", line.c_str());
        save->write(statBuf, line.size() + 1);
    }
}

/**
 * @brief  Save the log file
 * @note   used RTTI on fstream  to save the log file so at the end of the function the stream will be closed
 * @retval None
 */
void LogEngine::Finalize()
{
    using namespace std;
    fstream logFile(_logFile, ios_base::app);
    writeBuffer(&_buffer, &logFile);
}

void LogEngine::Trace(LogType type, std::string message)
{
    using namespace std;

    std::string log = LogTypeToString(type) + message + "\n";
    char buffer[log.size() + 1];
    sprintf(buffer, "%s\n", log.c_str());
    fmt::print(fmt::fg(LogTypeToColor(type)), log);
    _buffer.write(buffer, log.size() + 1);
}
