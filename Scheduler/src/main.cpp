#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/type_traits/promote.hpp>
#include <iostream>

boost::program_options::variables_map & optionsMap = *new boost::program_options::variables_map();
int main(int argc, char ** argv)
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

}