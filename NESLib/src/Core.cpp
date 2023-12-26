#include "LogEngine.hpp"
#include <Core.hpp>
#include <cstring>
#include <fmt/core.h>
void _panic(const char *message)
{
    if (LogEngine::Instance() == nullptr)
    {
        printf("%s", message); // best effor print
    }
    else
    {
        LogEngine::Instance()->Trace(LogType::EXCEPTION, message);
        LogEngine::Instance()->Flush();
    }
    while (1)
    {
    } // trap
}

void _panic(const char *message, const char *file, int line)
{
    char buffer[512];
    auto str = fmt::format("({}:{}){}", file, line, message);
    memcpy(buffer, str.c_str(), str.size() >= 512 ? 512 : str.size());
    str.clear();
    _panic(buffer);
}

void _panic(std::string message, const char *file, int line)
{
    _panic(message.c_str(), file, line);
}
