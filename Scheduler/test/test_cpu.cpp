//
// Created by drfaust on 26/01/23.
//
#include "CPU.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "MockStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include <fmt/core.h>
#include <gtest/gtest.h>

TEST(TestCpu, test_arrival)
{
    LogEngine::CreateInstance("test.txt");
    Scheduler sched{"scheduler"};
    auto cpu = new Cpu(&sched);
    sched.AddStation(cpu);
    auto evt = sched.Create(10, 0);
    evt.Station = 3;
    sched.Schedule(evt);
    while (sched.ProcessNext().ServiceTime > 0)
        ;
}

TEST(TestCpu, test_flooding)
{
    Scheduler sched{"scheduler"};
    auto cpu = new Cpu(&sched);
    auto io1 = new MockStation(Stations::IO_1);
    auto io2 = new MockStation(Stations::IO_2);
    auto swap_out = new MockStation(Stations::SWAP_OUT);
    sched.AddStation(io1);
    sched.AddStation(io2);
    sched.AddStation(swap_out);
    sched.AddStation(cpu);
    for (int i = 0; i < 1000; i++)
    {
        auto evt = sched.Create(i * 5, 27);
        evt.Station = CPU;
        sched.Schedule(evt);
    }
    while (sched.HasEvents())
        sched.ProcessNext();
    cpu->Update();
    fmt::print("Cpu U:{}", cpu->Data()["utilization"].value()->Current());
}