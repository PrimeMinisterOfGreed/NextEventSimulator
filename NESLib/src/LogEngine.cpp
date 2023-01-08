#include "LogEngine.hpp"
#include <fstream>
#include <ios>
#include <streambuf>

ConsoleLogEngine* ConsoleLogEngine::_instance = nullptr;

std::string &LogTypeToString(LogType logType)
{
    switch (logType)
    {
    case LogType::EXCEPTION:
        return *new std::string("[Exception]");
    case LogType::RESULT:
        return *new std::string("[Result]");
    case LogType::INFORMATION:
        return *new std::string("[Information]");
    case LogType::TRANSFER:
        return *new std::string("[Transfer]");
    case LogType::DEBUG:
        return *new std::string("[Debug]");
    }
}

/**
 * @brief  Save the log file
 * @note   used RTTI on fstream  to save the log file so at the end of the function the stream will be closed
 * @retval None
 */
void ConsoleLogEngine::Finalize()
{
    using namespace std;
    fstream logFile(_logFile, ios_base::out | ios_base::app);
    char buffer[1024];
    while (!_buffer.eof())
    {
        _buffer.readsome(buffer, 1024);
        logFile.write(buffer, 1024);
    }
}

void ConsoleLogEngine::Trace(LogType type, std::string &message)
{
    if (_verbosity >= (int)type)
    {
        std::string log = LogTypeToString(type) + message;
        printf("%s", log.c_str());
        _buffer.write(log.c_str(),log.size());
    }
}
