#include "CPU.hpp"
#include "Enums.hpp"
#include "OperativeSystem.hpp"
#include "Options.hpp"
#include "rngs.hpp"
#include "rvgs.h"

Cpu::Cpu(ILogEngine *logger, IScheduler *scheduler, double timeSlice)
    : Station(logger, Stations::CPU), _timeSlice(timeSlice),
      _scheduler(scheduler) {

  burst = RandomStream::Global().GetStream([this](RandomStream &gen) -> double {
    auto x = RandomStream::Global().Random();
    return alpha * (1 / u1) * exp(-x / u1) + beta * (1 / u2) * exp(-x / u2);
  });

  serviceTime = RandomStream::Global().GetStream([this](auto& gen){
    return Exponential(1/(_timeSlice/1000));
  });
  _name = "CPU";
}

void Cpu::ProcessArrival(Event &evt) {
  if (evt.SubType == EventType::NO_EVENT) {
    Station::ProcessArrival(evt);
    evt.SubType = 'C';
    double processServiceTime = (*serviceTime)();
    evt.ServiceTime = processServiceTime;
  }
  if (!_eventUnderProcess.has_value()) {
    ManageProcess(evt, (*burst)());
    _scheduler->Schedule(evt);
  } else {
    _eventQueue.Enqueue(evt);
  }
}

void Cpu::ProcessDeparture(Event &evt) {
  if (evt.ServiceTime > 0) {
    evt.Type = EventType::ARRIVAL;
    evt.OccurTime = _clock;
    if (_sysClients > 1) {
      _eventQueue.Enqueue(evt);
    } else {

      _scheduler->Schedule(evt);
    }
  } else {
    Station::ProcessDeparture(evt);
    double probabilities[3] = {0, 0.65, 0.9};
    double num = Uniform(0.0, 1.0);
    int selected = 0;
    for (int i = 0; i < 3; i++)
      if (num >= probabilities[i])
        selected = i;
    evt.ArrivalTime = _clock;
    evt.OccurTime = _clock;
    evt.Type = EventType::ARRIVAL;
    switch (selected) {
    case 0:
      evt.Station = Stations::IO_1;
      break;

    case 1:
      evt.Station = Stations::IO_2;
      break;

    case 2:
      evt.Station = Stations::SWAP_OUT;
      break;
    }
    _scheduler->Schedule(evt);
  }
  if (_sysClients > 0) {
    auto nextEvt = _eventQueue.Dequeue();
    ManageProcess(nextEvt, (*burst)());
    _scheduler->Schedule(nextEvt);
  }
}

void Cpu::ManageProcess(Event &evt, double burst) {
  evt.ArrivalTime = _clock;
  evt.CreateTime = _clock;
  if (burst < evt.ServiceTime)
    evt.OccurTime = _clock + burst;
  else
    evt.OccurTime = _clock + evt.ServiceTime;
  evt.ServiceTime -= burst;
  evt.Type = EventType::DEPARTURE;
}

void Cpu::Reset() {
  Station::Reset();
  _eventQueue.Clear();
}
