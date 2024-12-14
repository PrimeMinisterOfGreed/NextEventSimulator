/**
 * @file LogEngine.cpp
 * @author matteo.ielacqua
 * @brief implementazione per le trace del logengine, maggiori informazioni a @see LogEngine.hpp
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "LogEngine.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>
#include <string>



void writeBuffer(std::istream &buffer, std::ostream &save)
{
    using namespace std;
    std::string line;
    while (std::getline(buffer, line))
    {
        auto formatted = fmt::format("{}\n", line);
        save.write(formatted.c_str(), formatted.size());
    }
}

/**
 * @brief  Save the log file
 * @note   used RTTI on fstream  to save the log file so at the end of the function the stream will be closed
 * @retval None
 */
void LogEngine::Flush()
{
    using namespace std;
    fstream logFile(_logFile, ios_base::app);
    writeBuffer(_buffer, logFile);
    _buffer.clear();
    _buffer.seekp(0);
}

void LogEngine::Trace(LogType type, std::string message)
{
    using namespace std;
    auto res = fmt::format("[{}]{}\n",type,message);
    printf("%s",res.c_str());
}
