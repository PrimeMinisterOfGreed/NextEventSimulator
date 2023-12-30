//
// Created by drfaust on 26/01/23.
//
#include "CPU.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Scheduler.hpp"
#include <gtest/gtest.h>

TEST(TestCpu, test_arrival)
{
    LogEngine::CreateInstance("test.txt");
    Scheduler sched{"scheduler"};
    auto cpu = new Cpu(&sched);
    sched.AddStation(cpu);
    auto evt = sched.Create(0, 0);
    evt.Station = 3;
    sched.Schedule(evt);
    while (sched.ProcessNext().ServiceTime > 0)
        ;
}