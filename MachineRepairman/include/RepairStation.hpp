#include "LogEngine.hpp"
#include "Station.hpp"


class RepairStation : public Station
{
  protected:
    void ProcessArrival(Event &evt) override;
    void ProcessDeparture(Event &evt) override;

  public:
    RepairStation();
};