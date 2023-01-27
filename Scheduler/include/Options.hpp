#pragma once
#include <boost/program_options/variables_map.hpp>
#include "Generators.hpp"

extern boost::program_options::variables_map & optionsMap;



extern std::shared_ptr<IGenerator> & streamGenerator;