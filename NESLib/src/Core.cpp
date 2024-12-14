/**
 * @file Core.cpp
 * @author matteo ielacqua (you@domain.com)
 * @brief implementazione dei panic handler, maggiori dettagli in @see Core.hpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "LogEngine.hpp"
#include "Core.hpp"
#include <cstring>
[[noreturn]] void _panic(const char *message)
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
    char buffer[513]{};
    auto str = fmt::format("({}:{}){}\0", file, line, message);
    memcpy(buffer, str.c_str(), str.size() >= 512 ? 512 : str.size());
    str.clear();
    _panic(buffer);
}

void _panic(std::string message, const char *file, int line)
{
    _panic(message.c_str(), file, line);
}


void abort()
{
    printf("[CriticalException] Error abort called");
    while (1)
    {
       //trap 
    }
}
