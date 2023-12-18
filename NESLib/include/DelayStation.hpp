#include "ISimulator.hpp"
#include "Station.hpp"
#include <functional>

struct DelayStation : public Station
{
  protected:
    IScheduler *_scheduler;
    std::function<double()> _delayTime;
    int _numclients = 0;

  public:
    template <typename F>
    DelayStation(IScheduler *scheduler, std::string name, int numclients, F &&delayFnc)
        : Station(name, 0), _scheduler(scheduler), _delayTime(delayFnc), _numclients(numclients)
    {
    }
    void Initialize() override;
    void ProcessArrival(Event &evt) override;
};