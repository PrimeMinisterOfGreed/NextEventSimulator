#include "DataCollector.hpp"
#include "DataWriter.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"
#include "LogEngine.hpp"
#include "OperativeSystem.hpp"
#include "Station.hpp"
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
    OS os = OS(2.7, 10);
    os.Schedule(Event("END", EventType::END, 0, 1000, 0, 0, 0));
    DataWriter::Instance().header = os.Data().Header();
    auto handler = [&os](auto s) {
        if (os.arrivals() == 0)
            return;
        os.Update();
        DataWriter::Instance().WriteLine(os.Data().Csv());

        for (Station *station : s)
        {
            station->Update();
            if (station->arrivals() > 0)
                DataWriter::Instance().WriteLine(station->Data().Csv());
        }
    };
    os.OnProcessFinished = FunctionPointer<void, std::vector<Station *>>(handler);
    os.OnEventProcess = FunctionPointer<void, std::vector<Station *>>(handler);

    os.Execute();
    DataWriter::Instance().Flush();
    LogEngine::Instance()->Finalize();
}