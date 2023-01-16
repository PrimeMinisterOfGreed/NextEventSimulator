#include "MachineRepairman.hpp"
#include "Event.hpp"
#include "FCFSStation.hpp"
#include "LogEngine.hpp"
#include "Station.hpp"
#include <vector>

void MachineRepairman::Execute()
{
    bool isEnd = false;
    while (!isEnd)
    {
        _logger->TraceTransfer("EventList:{}\n", _eventList.ToString());
        auto nextEvt = &_eventList.Dequeue();
        auto type = nextEvt->Type;
        _repairStation->Process(nextEvt);
        _clock = nextEvt->OccurTime;

        if (type == EventType::ARRIVAL)
        {
            double serviceTime = _provider->GetService();
            double interArrival = _provider->GetInterArrival();
            double arrival = _provider->GetArrival();
            Schedule(new Event(makeformat("J{}S1", Event::GeneratedNodes), EventType::ARRIVAL, _clock, arrival,
                               serviceTime, arrival));
        }
        else if (type == EventType::END)
        {
            isEnd = true;
            Report();
        }
    }
}

void MachineRepairman::Report()
{
    auto stats = _repairStation->GetStatistics();
    _logger->TraceResult("Statistics\n{}", stats.ToString());
}

std::vector<StationStatistic> MachineRepairman::GetStats() const
{
    return *new std::vector<StationStatistic>({_repairStation->GetStatistics()});
}

void MachineRepairman::Schedule(Event *event)
{
    _eventList.Insert(event, [](const Event &a, const Event &b) { return a.OccurTime > b.OccurTime; });
}

void MachineRepairman::Reset()
{
    _repairStation->Reset();
    _clock = 0.0;
}

MachineRepairman::MachineRepairman(ILogEngine *logger, IDataProvider *provider, double endTime)
    : _repairStation(new FCFSStation(logger, this, 1)), _provider(provider), _logger(logger)
{
    double arrival = provider->GetArrival();
    double service = provider->GetService();
    Schedule(new Event(makeformat("J{}S1", 0), EventType::ARRIVAL, _clock, arrival, service, arrival));
    Schedule(new Event(makeformat("END"), EventType::END, _clock, endTime, 0, 0));
}
