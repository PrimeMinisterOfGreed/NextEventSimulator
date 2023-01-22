#include "CPU.hpp"
#include "Enums.hpp"
#include "rngs.hpp"
#include "OperativeSystem.hpp"
Cpu::Cpu(ILogEngine *logger, IScheduler *scheduler) : FCFSStation(logger, scheduler, Stations::CPU)
{

}

void Cpu::ProcessArrival(Event *evt)
{
    FCFSStation::ProcessArrival(evt);

}

void Cpu::ProcessDeparture(Event *evt)
{
    FCFSStation::ProcessDeparture(evt);

    double probabilities[3] = {0.65, 0.9, 1};

    // maybe you should get a new stream over there
    double num = Random();
    int selected = 0;
    for (int i = 0; i < 3; i++)
        if (num > probabilities[i])
            selected = i;
    switch (selected) {
    case 0:
        //GOTO IO1
        break;

    case 1:
        //GOTO IO2
        break;

    case 2:
        //GOTO swapOut
        break;
    }
}
