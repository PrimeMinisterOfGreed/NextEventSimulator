#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "LogEngine.hpp"
#include "OperativeSystem.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/type_traits/promote.hpp>
#include <iostream>
#include <vector>

boost::program_options::variables_map &optionsMap = *new boost::program_options::variables_map();

int main(int argc, char **argv)
{
    using namespace boost::program_options;
    options_description descr("Allowed options");
    descr.add_options()("help", "display this help");
    store(parse_command_line(argc, argv, descr), optionsMap);
    notify(optionsMap);
    if (optionsMap.count("help"))
    {
        std::cout << descr << std::endl;
    }
    LogEngine::CreateInstance(3, "simulation.txt");
    OS os = OS();
    auto endTime = SystemParameters::Parameters().endTime;
    auto probeTime = SystemParameters::Parameters().probeInterval;
    os.Schedule(Event("END", EventType::END, 0, SystemParameters::Parameters().endTime, 0, 0, 0));
    for (int i = 0; i < (endTime / probeTime); i++)
    {
        os.Schedule(Event("PROBE", EventType::PROBE, 0, (i + 1) * probeTime, 0, i * probeTime));
    }
    os.Execute();
    DataWriter::Instance().Flush();
    LogEngine::Instance()->Finalize();
}