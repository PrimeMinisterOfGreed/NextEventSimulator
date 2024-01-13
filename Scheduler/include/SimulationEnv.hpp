#pragma  once
#include "DataWriter.hpp"
#include "Event.hpp"
#include "ISimulator.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "OperativeSystem.hpp"
#include "Shell/SimulationShell.hpp"
#include "Station.hpp"
#include "Strategies/RegenerationPoint.hpp"
#include "Strategies/TaggedCustomer.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>


extern SimulationShell shell;
void SetupEnvironment();