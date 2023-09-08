#include "DataCollector.hpp"
#include "DataWriter.hpp"
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
    DataWriter::Instance().header = os.Data().Header();
    auto handler = [&os](auto s) {
        os.Update();
        DataWriter::Instance().Write(makeformat("{}\n", os.Data().Csv()));

        for (Station *station : s)
        {
            station->Update();
            DataWriter::Instance().Write(makeformat("{}\n", station->Data().Csv()));
        }
    };
    os.OnProcessFinished.Attach(handler);
    os.OnProcessFinished.Attach(handler);
    os.Execute();
    DataWriter::Instance().Flush();
}