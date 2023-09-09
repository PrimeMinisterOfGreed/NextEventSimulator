#include "Event.hpp"
#include "FCFSStation.hpp"
#include "Station.hpp"
#include "ToString.hpp"
#include <optional>

class RoundRobinStation : public FCFSStation
{
  public:
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;
};