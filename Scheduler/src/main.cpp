#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "LogEngine.hpp"
#include "OperativeSystem.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include <cstdio>
#include <iostream>
#include <vector>

int main(int argc, char **argv)
{

    LogEngine::CreateInstance("simulation.txt");
    OS os = OS();
    os.Initialize();
    bool _end = false;
    while (!_end)
    {

        os.Execute();
    }
}