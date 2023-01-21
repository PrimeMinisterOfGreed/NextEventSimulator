#include "CPU.hpp"
#include "Enums.hpp"
#include "rngs.hpp"
#include "OperativeSystem.hpp"
CPU::CPU(ILogEngine *logger, IScheduler *scheduler) : FCFSStation(logger, scheduler, Stations::CPU)
{

}

void CPU::ProcessArrival(Event *evt)
{
    FCFSStation::ProcessArrival(evt);

}

void CPU::ProcessDeparture(Event *evt)
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
